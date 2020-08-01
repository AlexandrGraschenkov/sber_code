//
//  SberCode.cpp
//  code_mac
//
//  Created by Alexander Graschenkov on 31.07.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#include "SberCodeAPI.hpp"
#include <config.h>
//#include <zbar.h>
#include <opencv2/imgproc.hpp>
#include <zxing/MultiFormatReader.h>
#include <zxing/common/GreyscaleLuminanceSource.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/multi/GenericMultipleBarcodeReader.h>

//using namespace zbar;
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
Recognizer::Recognizer() {
//    zScanner = zbar_image_scanner_create();
//    zImage = zbar_image_create();
//
//    zbar_image_scanner_set_config(zScanner, ZBAR_NONE, ZBAR_CFG_X_DENSITY, 3);
//    zbar_image_scanner_set_config(zScanner, ZBAR_NONE, ZBAR_CFG_Y_DENSITY, 3);
//    zbar_image_scanner_enable_cache(zScanner, true);
    
}

Recognizer::~Recognizer() {
    if(zScanner) {
//        zbar_image_scanner_destroy(zScanner);
//        zScanner = NULL;
//        zbar_image_destroy(zImage);
//        zImage = NULL;
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
    
    // ZBAR
//    zbar_image_set_data(zImage, grayImg.data, grayImg.size().area(), nullptr);
//    zbar_image_set_format(zImage, zbar_fourcc('Y','8','0','0'));
//    zbar_image_set_size(zImage, grayImg.cols, grayImg.rows);
//
//    zbar_scan_image(zScanner, zImage);
//    const zbar_symbol_set_t *symSet = zbar_image_scanner_get_results(zScanner);
//    vector<Code> result = Code::parseResult(symSet);
    
    // ZXING
//    Ref<LuminanceSource> imgSource = MatSource::create(grayImg);
//    Ref<LuminanceSource>(new GreyscaleLuminanceSource(greyData_, dataWidth_, dataHeight_, left, top, width, height))
    ArrayRef<char> dataRef((char *)grayImg.data, grayImg.size().area());
    Ref<LuminanceSource> imgSource = Ref<LuminanceSource>(new GreyscaleLuminanceSource(dataRef, grayImg.cols, grayImg.rows, 0, 0, grayImg.cols, grayImg.rows));
    Ref<Binarizer> binarizer = Ref<Binarizer>(new HybridBinarizer(imgSource));
    Ref<BinaryBitmap> bitmap = Ref<BinaryBitmap>(new BinaryBitmap(binarizer));
    
    MultiFormatReader delegate;
    multi::GenericMultipleBarcodeReader reader(delegate);
    
    DecodeHints hints(SBER_HINT);
    vector<Ref<Result>> zxingResults = reader.decodeMultiple(bitmap, hints);
    vector<Code> result = Code::parseResult(zxingResults);
//    if (result2.size()) {
//        result.insert(result.end(), result2.begin(), result2.end());
//    }
    
    return result;
}
}// namespace SberCode
