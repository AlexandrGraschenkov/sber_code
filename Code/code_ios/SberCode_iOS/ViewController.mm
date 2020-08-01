//
//  ViewController.m
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 01.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import "ViewController.h"
#import "Camera.h"
#import <sber_code/sber_code.h>
#import "Camera.h"
#include "fps.hpp"
#import "UIView+Mat.h"
#include "VisualizeBehavior.h"


@interface ViewController () <CameraDelegate> {
    SberCode::Recognizer recognizer;
    Camera *cam;
    FPS *fps;
    FPSIteration *fpsIter;
    VisualizeBehavior *visBehavior;
}
@property (nonatomic, weak) IBOutlet UILabel *fpsLabel;
@property (nonatomic, weak) IBOutlet UIView *debugView;
@property (atomic, assign) BOOL showDebug;

@end

@implementation ViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

    recognizer.tracking = true;
    #if TARGET_OS_IPHONE
    cam = [[Camera alloc] init];
    cam.delegate = self;
    [self.view.layer insertSublayer:cam.previewLayer atIndex:0];
    cam.previewLayer.frame = self.view.bounds;
    
    visBehavior = [[VisualizeBehavior alloc] initWithLayer:cam.previewLayer];
    #endif
        
    fps = new FPS();
    fpsIter = new FPSIteration();
    self.showDebug = YES;
}

- (IBAction)debugSwitchChanged:(UISwitch *)sender {
    self.showDebug = sender.isOn;
}

- (IBAction)dismissPressed:(id)sender {
    [self dismissViewControllerAnimated:true completion:nil];
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    
    cam.previewLayer.frame = self.view.bounds;
}

- (void)cameraImageRecordered:(cv::Mat&)img {
    fpsIter->start();
    std::vector<SberCode::Code> recognized = recognizer.recognize(img, SberCode::ImageFormat_Gray);
    fpsIter->end();
    fps->tick();
    if (recognized.size()) {
        NSString *codes = @"";
        for (SberCode::Code &c : recognized) {
            codes = [codes stringByAppendingFormat:@"%@ ", [NSString stringWithCString:c.typeName.c_str() encoding:NSUTF8StringEncoding]];
        }
        NSLog(@"Found %ld codes> %@", recognized.size(), codes);
    }
    
    cv::Size imgSize = img.size();
    static int recogCount = 0;
    recogCount++;
    double fpsVal = fps->getFPS();
    double fpsProcess = fpsIter->getFPS();
    dispatch_async(dispatch_get_main_queue(), ^{
        self.fpsLabel.text = [NSString stringWithFormat:@"%.2lf < FPS\n%.2lf <Process", fpsVal, fpsProcess];
        [self->visBehavior updateRecognitions:recognized size:imgSize];
    });
}
@end
