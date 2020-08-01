//
//  SberCode.cpp
//  code_mac
//
//  Created by Alexander Graschenkov on 31.07.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#include "SberCodeAPI.hpp"
#include <config.h>
#include <zbar.h>
#include <opencv2/imgproc.hpp>
#include <zxing/MultiFormatReader.h>
#include <zxing/common/GreyscaleLuminanceSource.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/multi/GenericMultipleBarcodeReader.h>

using namespace zbar;
using namespace zxing;
using namespace cv;
using namespace std;

namespace SberCode {
Recognizer::Recognizer(CodeType types) {
    zScanner = zbar_image_scanner_create();
    zImage = zbar_image_create();
    
    zbar_image_scanner_set_config(zScanner, ZBAR_NONE, ZBAR_CFG_X_DENSITY, 3);
    zbar_image_scanner_set_config(zScanner, ZBAR_NONE, ZBAR_CFG_Y_DENSITY, 3);
    zbar_image_scanner_enable_cache(zScanner, true);
    
}

Recognizer::~Recognizer() {
    if(zScanner) {
        zbar_image_scanner_destroy(zScanner);
        zScanner = NULL;
        zbar_image_destroy(zImage);
        zImage = NULL;
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
    zbar_image_set_data(zImage, grayImg.data, grayImg.size().area(), nullptr);
    zbar_image_set_format(zImage, zbar_fourcc('Y','8','0','0'));
    zbar_image_set_size(zImage, grayImg.cols, grayImg.rows);
    
    zbar_scan_image(zScanner, zImage);
    const zbar_symbol_set_t *symSet = zbar_image_scanner_get_results(zScanner);
    vector<Code> result = Code::parseResult(symSet);
    
    // ZXING
    ArrayRef<char> dataRef((char *)grayImg.data, grayImg.size().area());
    Ref<LuminanceSource> imgSource = Ref<LuminanceSource>(new GreyscaleLuminanceSource(dataRef, grayImg.cols, grayImg.rows, 0, 0, 0, 0));
    Ref<Binarizer> binarizer = Ref<Binarizer>(new HybridBinarizer(imgSource));
    DecodeHints hints(DecodeHints::DATA_MATRIX_HINT | DecodeHints::AZTEC_HINT);
    
    MultiFormatReader delegate;
    multi::GenericMultipleBarcodeReader reader(delegate);
    Ref<BinaryBitmap> bitmap = Ref<BinaryBitmap>(new BinaryBitmap(binarizer));
    vector<Ref<Result>> zxingResults = reader.decodeMultiple(bitmap, hints);
    vector<Code> result2 = Code::parseResult(zxingResults);
    if (result2.size()) {
        result.insert(result.end(), result2.begin(), result2.end());
    }
    
    return result;
}
}// namespace SberCode
