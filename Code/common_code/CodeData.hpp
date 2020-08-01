//
//  CodeData.hpp
//  code_mac
//
//  Created by Alexander Graschenkov on 31.07.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#ifndef CodeData_hpp
#define CodeData_hpp

#include <stdio.h>
#include <opencv2/core.hpp>

//namespace zbar {
//    struct zbar_symbol_s;
//    typedef struct zbar_symbol_s zbar_symbol_t;
//
//    struct zbar_symbol_set_s;
//    typedef struct zbar_symbol_set_s zbar_symbol_set_t;
//}; // namespace zbar

namespace zxing {
    template<typename T> class Ref;
    class Result;
}; // namespace zxing


namespace SberCode {

struct Code {
    std::string typeName;
    std::string message;
    std::vector<cv::Point> location;
    
    double detectTime;
    int trackId;
    
    Code(){};
//    Code(const zbar::zbar_symbol_t *);
//    static std::vector<Code> parseResult(const zbar::zbar_symbol_set_t *);
    static std::vector<Code> parseResult(const std::vector<zxing::Ref<zxing::Result>> &);
};

} // namespace SberCode

#endif /* CodeData_hpp */
