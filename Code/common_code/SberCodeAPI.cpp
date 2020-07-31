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

using namespace zbar;
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
    
    zbar_image_set_data(zImage, grayImg.data, grayImg.size().area(), nullptr);
    zbar_image_set_format(zImage, zbar_fourcc('Y','8','0','0'));
    zbar_image_set_size(zImage, grayImg.cols, grayImg.rows);
    
    zbar_scan_image(zScanner, zImage);
    const zbar_symbol_set_t *symSet = zbar_image_scanner_get_results(zScanner);
    vector<Code> result = Code::parseResult(symSet);
    return result;
}
}// namespace SberCode
