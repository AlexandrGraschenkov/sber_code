//
//  CodeData.cpp
//  code_mac
//
//  Created by Alexander Graschenkov on 31.07.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#include "CodeData.hpp"
#include <zbar.h>
#include <opencv2/imgproc.hpp>

using namespace zbar;
using namespace std;
using namespace cv;

namespace SberCode {

std::vector<Code> Code::parseResult(const zbar_symbol_set_t *recognized) {
    const zbar_symbol_t *sym = zbar_symbol_set_first_unfiltered(recognized);
    vector<Code> result;
    for (int i = 0; sym != nullptr; i++, sym = zbar_symbol_next(sym)) {
        result.push_back(Code(sym));
    }
    return result;
};


Code::Code(const zbar::zbar_symbol_t *sym) {
    message = string(zbar_symbol_get_data(sym));
    quality = zbar_symbol_get_quality(sym);
    
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
}

}; // namespace SberCode
