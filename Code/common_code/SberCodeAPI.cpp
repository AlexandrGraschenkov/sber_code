//
//  SberCode.cpp
//  code_mac
//
//  Created by Alexander Graschenkov on 31.07.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#include "SberCodeAPI.hpp"
#include <config.h>

#ifdef USE_ZBAR
#include <zbar.h>
#endif

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <zxing/MultiFormatReader.h>
#include <zxing/common/GreyscaleLuminanceSource.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/multi/GenericMultipleBarcodeReader.h>
#include <sys/time.h>
#include "code_utils/hungarian.hpp"
#include <unordered_set>

#ifdef USE_ZBAR
using namespace zbar;
#endif
using namespace zxing;
using namespace cv;
using namespace std;

const DecodeHints SBER_HINT(
DecodeHints::QR_CODE_HINT |
DecodeHints::DATA_MATRIX_HINT |
DecodeHints::AZTEC_HINT |
                            
DecodeHints::CODE_39_HINT |
DecodeHints::CODE_93_HINT |
DecodeHints::CODE_128_HINT |
DecodeHints::CODABAR_HINT |
DecodeHints::UPC_A_HINT |
DecodeHints::UPC_E_HINT |
DecodeHints::EAN_13_HINT |
DecodeHints::EAN_8_HINT
);

namespace SberCode {


static double getTimestamp() {
    struct timeval now;
    gettimeofday(&now, nullptr);
    return now.tv_usec / 1000000.0 + now.tv_sec;
}

Recognizer::Recognizer() {
    clahe = createCLAHE(4, Size(3, 3));
#ifdef USE_ZBAR
    zScanner = zbar_image_scanner_create();
    zImage = zbar_image_create();

    zbar_image_scanner_set_config(zScanner, ZBAR_NONE, ZBAR_CFG_X_DENSITY, 3);
    zbar_image_scanner_set_config(zScanner, ZBAR_NONE, ZBAR_CFG_Y_DENSITY, 3);
    zbar_image_scanner_enable_cache(zScanner, true);
#endif
    
}

Recognizer::~Recognizer() {
    if(zScanner) {
#ifdef USE_ZBAR
        zbar_image_scanner_destroy(zScanner);
        zScanner = NULL;
        zbar_image_destroy(zImage);
        zImage = NULL;
#endif
    }
}

std::vector<Code> Recognizer::recognize(const cv::Mat &frame, ImageFormat format) {
    
    if (format == ImageFormat_Gray) {
        frame.copyTo(grayImg);
    } else if (format == ImageFormat_RGBA) {
        cvtColor(frame, grayImg, COLOR_RGBA2GRAY);
    } else if (format == ImageFormat_RGB) {
        cvtColor(frame, grayImg, COLOR_RGB2GRAY);
    }
    
//    threshold(tempImg, grayImg, 0, 255, THRESH_OTSU);
//    grayImg = tempImg;
//    clahe->apply(tempImg, grayImg);
//    imshow("orig", tempImg);
//    imshow("contrast", grayImg);
    
    
    vector<Code> result;
    
    // ZBAR
#ifdef USE_ZBAR
    zbar_image_set_data(zImage, grayImg.data, grayImg.size().area(), nullptr);
    zbar_image_set_format(zImage, zbar_fourcc('Y','8','0','0'));
    zbar_image_set_size(zImage, grayImg.cols, grayImg.rows);

    zbar_scan_image(zScanner, zImage);
    const zbar_symbol_set_t *symSet = zbar_image_scanner_get_results(zScanner);
    result = Code::parseResult(symSet);
#endif
    cout << result.size() << endl;
    
    // ZXING
    ArrayRef<char> dataRef((char *)grayImg.data, grayImg.size().area());
    Ref<LuminanceSource> imgSource = Ref<LuminanceSource>(new GreyscaleLuminanceSource(dataRef, grayImg.cols, grayImg.rows, 0, 0, grayImg.cols, grayImg.rows));
    Ref<Binarizer> binarizer = Ref<Binarizer>(new HybridBinarizer(imgSource));
    Ref<BinaryBitmap> bitmap = Ref<BinaryBitmap>(new BinaryBitmap(binarizer));
    
    MultiFormatReader delegate;
    multi::GenericMultipleBarcodeReader reader(delegate);
    
    DecodeHints hints(SBER_HINT);
    vector<Ref<Result>> zxingResults;
    try {
//        zxingResults = reader.decodeMultiple(bitmap, hints);
    } catch (std::exception exp) {
        cout << "Some err" << endl;
    }
    vector<Code> result2 = Code::parseResult(zxingResults);
    if (result2.size()) {
//        result.insert(result.end(), result2.begin(), result2.end());
    }
    
//    filterResult(result);
    double currTime = getTimestamp();
    for (Code &c : result) {
        c.detectTime = currTime;
    }
    
    if (tracking) {
        doTrack(result);
    }
    
    
    if (result.size()) {
        Rect r = cv::boundingRect(result[0].location);
        cout << result.size() << result[0].message << " Rect " << r.x << " " << r.y << " " << r.width << " " << r.height << endl;
    }
    
    return result;
}


Point2f getCenter(const std::vector<Point> &vec) {
    if (vec.size() == 0) {
        return {};
    }
    Point center = {};
    for (Point p : vec) {
        center += p;
    }
    center = center / (int)vec.size();
    return center;
}

void Recognizer::filterResult(std::vector<Code> &recognizedCodes) {
    // remove too small codes
    for (int i = 0; i < recognizedCodes.size(); i++) {
        Rect foundedRect = boundingRect(recognizedCodes[i].location);
        if (foundedRect.width <= 2 || foundedRect.height <= 2) {
            recognizedCodes.erase(recognizedCodes.begin() + i);
            i--;
        }
    }
    
    // remove dublicates
    for (int i = 0; i < recognizedCodes.size(); i++) {
        for (int j = i+1; j < recognizedCodes.size(); i++) {
            Point c1 = getCenter(recognizedCodes[i].location);
            Point c2 = getCenter(recognizedCodes[j].location);
            double d1 = pointPolygonTest(recognizedCodes[i].location, c2, false);
            double d2 = pointPolygonTest(recognizedCodes[j].location, c1, false);
            if (max(d1, d2) > 0) {
                recognizedCodes.erase(recognizedCodes.begin() + j);
                j--;
            }
        }
    }
}

void Recognizer::doTrack(std::vector<Code> &recognizedCodes) {
    Mat1f distances((int)historyTrack.size(), (int)recognizedCodes.size());

    for (int i0 = 0; i0 < historyTrack.size(); i0++) {
        Code &hc = historyTrack[i0];
        for (int i1 = 0; i1 < recognizedCodes.size(); i1++) {
            Code &c = recognizedCodes[i1];
            float dist = 0;
            if (hc.typeName != c.typeName) dist += 1000;
            if (hc.message != c.message) dist += 100;
            Point hcCenter = getCenter(hc.location);
            Point cCenter = getCenter(c.location);
            double centerDist = norm(hcCenter - cCenter);
            dist += centerDist / 100;
            distances.at<float>(i0, i1) = dist;
        }
    }
    
    static matching::HungarianAlgorithm matchAlgo;
    vector<int> assigments = {};
    if (distances.size().area())
        matchAlgo.solve(distances, assigments);
    
    unordered_set<int> unusedCodes;
    for (int i = 0; i < recognizedCodes.size(); i++) {
        unusedCodes.insert(i);
    }
    for (int i = 0; i < assigments.size(); i++) {
        int newIdx = assigments[i];
        if (newIdx >= 0 && distances.at<float>(i, newIdx) < 100) {
            recognizedCodes[newIdx].trackId = historyTrack[i].trackId;
            historyTrack[i] = recognizedCodes[newIdx];
            unusedCodes.erase(newIdx);
        }
    }
    
    
    // cleanup old track
    double currTime = getTimestamp();
    for (int i = (int)historyTrack.size()-1; i >= 0; i--) {
        if (currTime - historyTrack[i].detectTime > keepTrackSec) {
            historyTrack.erase(historyTrack.begin() + i);
        }
    }
    
    // create new unmatched code tracks
    for (int newTrackIdx : unusedCodes) {
        recognizedCodes[newTrackIdx].trackId = ++lastCodeId;
        historyTrack.push_back(recognizedCodes[newTrackIdx]);
    }
}
}// namespace SberCode
