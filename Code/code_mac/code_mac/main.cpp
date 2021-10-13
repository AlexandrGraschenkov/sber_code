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

vector<string> getImagesInFolder(string folderPath) {
    vector<string> fn;
    glob("/Users/alex/Downloads/Штрихкоды/Тест/*.png", fn, false);
    return fn;
}

void processVideo(std::string vidPath) {
    Recognizer rec;
    rec.tracking = true;
    
    VideoCapture cap(vidPath);
    Mat frame;
    int foundCount = 0;
    while (cap.read(frame)) {
        vector<Code> codes = rec.recognize(frame, ImageFormat_RGB);
        foundCount += codes.size();
        for (Code &c : codes) {
            strokePoly(frame, c.location, CV_RGB(255, 200, 0), 3);
            cout << "TYPE: " << c.typeName << " #" << c.trackId << " > " << c.message << endl;
        }
        imshow("founded",frame);
        waitKey(1);
    }
    cout << "Total founded codes " << foundCount << endl;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
//    processVideo("/Users/alex/Downloads/IMG_4231.MOV");
//    return 0;
//    string imgPath = "/Users/alex/Downloads/sber_code_data/ean-13_0.jpeg";
//    string imgPath = "/Users/alex/Downloads/sber_code_data/code-128_0.jpeg";
//    string imgPath = "/Users/alex/Downloads/photo_2020-07-31 15.43.35.jpeg";
//string imgPath = "/Users/alex/Downloads/sber_code_data/data_matrix.jpeg";
string imgPath = "/Users/alex/Downloads/Штрихкоды/Тест/QR_win1251_02.png";
    
    Mat img = imread(imgPath);
    
    Recognizer rec;
    
    vector<string> checkImages;
    glob("/Users/alex/Downloads/Штрихкоды/Тест/*.png", checkImages, false);
    checkImages.insert(checkImages.begin(), imgPath);
    for (string imgPath : checkImages) {
        cout << imgPath << endl;
        Mat img = imread(imgPath);
        vector<Code> codes = rec.recognize(img, ImageFormat_RGB);
        for (Code &c : codes) {
            strokePoly(img, c.location, CV_RGB(255, 200, 0), 3);
            cout << "TYPE: " << c.typeName << " > " << c.message << endl;
        }
        imshow("founded", img);
        auto k = waitKey();
        if (k == 'q') break;
    }
    
    return 0;
}
