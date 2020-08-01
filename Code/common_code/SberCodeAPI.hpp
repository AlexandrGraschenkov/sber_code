//
//  SberCode.hpp
//  code_mac
//
//  Created by Alexander Graschenkov on 31.07.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#ifndef SberCode_hpp
#define SberCode_hpp

#include <stdio.h>
#include "CodeData.hpp"
#include <opencv2/imgproc.hpp>


namespace zbar {
struct zbar_image_scanner_s;
typedef struct zbar_image_scanner_s zbar_image_scanner_t;

struct zbar_image_s;
typedef struct zbar_image_s zbar_image_t;
}

namespace SberCode {

enum ImageFormat {
    ImageFormat_Gray,
    ImageFormat_RGBA,
    ImageFormat_RGB
};

class Recognizer {
public:
    Recognizer();
    ~Recognizer();
    bool tracking = false;
    
    std::vector<Code> recognize(const cv::Mat &frame, ImageFormat format);
    
private:
    zbar::zbar_image_scanner_t *zScanner = nullptr;
    zbar::zbar_image_t *zImage = nullptr;
    cv::Mat grayImg, tempImg;
    std::vector<Code> historyTrack;
    int lastCodeId = 0;
    double keepTrackSec = 5.0;
    cv::Ptr<cv::CLAHE> clahe;
    
    void doTrack(std::vector<Code> &recognizedCodes);
};


}// namespace SberCode

#endif /* SberCode_hpp */
