//
//  main.cpp
//  code_mac
//
//  Created by Alexander Graschenkov on 31.07.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#include <iostream>
#include "SberCodeAPI.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace SberCode;
using namespace std;
using namespace cv;

void strokePoly(cv::Mat &img, const std::vector<Point> &poly, Scalar color, int lineWidth = 1) {
    for (int i = 0; i < poly.size(); i++) {
        int ii = (i + 1) % poly.size();
        line(img, poly[i], poly[ii], CV_RGB(255, i * 10, 0), lineWidth);
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
//    string imgPath = "/Users/alex/Downloads/sber_code_data/ean-13_0.jpeg";
//    string imgPath = "/Users/alex/Downloads/sber_code_data/code-128_0.jpeg";
    string imgPath = "/Users/alex/Downloads/photo_2020-07-31 15.43.35.jpeg";
    
    Mat img = imread(imgPath);
    
    Recognizer rec(CodeType_None);
    vector<Code> codes = rec.recognize(img, ImageFormat_RGB);
    string msg = "null";
    if (codes.size()) {
        msg = "(" + codes[0].typeName + ") > " + codes[0].message;
    }
    cout << "Res: " << msg << endl;
    
    for (Code &c : codes) {
        strokePoly(img, c.location, CV_RGB(255, 200, 0), 3);
    }
    imshow("founded", img);
    waitKey();
    
    return 0;
}
