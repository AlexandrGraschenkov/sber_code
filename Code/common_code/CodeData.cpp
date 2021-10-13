//
//  CodeData.cpp
//  code_mac
//
//  Created by Alexander Graschenkov on 31.07.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#define USE_ZBAR

#include "CodeData.hpp"
#ifdef USE_ZBAR
#include <zbar.h>
#endif
#include <opencv2/imgproc.hpp>
#include <zxing/Result.h>
#include <zxing/BarcodeFormat.h>
#include <iconv.h>
#include <unordered_map>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

#ifdef USE_ZBAR
using namespace zbar;
#endif
using namespace zxing;
using namespace std;
using namespace cv;

namespace SberCode {


std::vector<Code> Code::parseResult(const std::vector<zxing::Ref<zxing::Result>> &recognized) {
    vector<Code> result;
    for (int i = 0; i < recognized.size(); i++) {
        Code c;
        auto format = recognized[i]->getBarcodeFormat();
        c.typeName = string(BarcodeFormat::barcodeFormatNames[(int)format]);
        c.message = recognized[i]->getText()->getText();
        
        for (const auto &recP : recognized[i]->getResultPoints()->values()) {
            Point p(recP->getX(), recP->getY());
            c.location.push_back(p);
        }
        
        if (c.location.size() == 2) {
            Point vec = c.location[0] - c.location[1];
            vec = vec / 10;
            vec = Point(-vec.y, vec.x);
            vector<Point> fourPoints = {
                {c.location[0] + vec},
                {c.location[1] + vec},
                {c.location[1] - vec},
                {c.location[0] - vec}
            };
            c.location = fourPoints;
        }
        if (c.typeName == "DATA_MATRIX" && c.location.size() == 4) {
            c.location = {c.location[0], c.location[1], c.location[3], c.location[2]};
        }
        result.push_back(c);
    }
    return result;
}

std::vector<Code> Code::parseResult(const zbar_symbol_set_t *recognized) {
#ifdef USE_ZBAR
    const zbar_symbol_t *sym = zbar_symbol_set_first_unfiltered(recognized);
    vector<Code> result;
    for (int i = 0; sym != nullptr; i++, sym = zbar_symbol_next(sym)) {
        result.push_back(Code(sym));
    }
    return result;
#endif
    return {};
};

Code::Code(const zbar::zbar_symbol_t *sym) {
#ifdef USE_ZBAR
    message = string(zbar_symbol_get_data(sym));
    
    auto type = zbar_symbol_get_type(sym);
    typeName = zbar_get_symbol_name(type);
    
    
    int n = zbar_symbol_get_loc_size(sym);
    for(int i = 0; i < n; i++) {
        int x = zbar_symbol_get_loc_x(sym, i);
        int y = zbar_symbol_get_loc_y(sym, i);
        location.push_back(cv::Point(x, y));
    }
    
    if (location.size() > 4) {
        RotatedRect rr = minAreaRect(location);
        Point2f rectPoints[4]; rr.points(rectPoints);
        location.resize(4);
        for (int i = 0; i < 4; i++) {
            location[i] = rectPoints[i];
        }
    }
#endif
}

}; // namespace SberCode
