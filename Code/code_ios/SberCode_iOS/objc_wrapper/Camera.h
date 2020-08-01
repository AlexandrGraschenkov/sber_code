//
//  Camera.h
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 01.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <opencv2/core.hpp>


@protocol CameraDelegate
- (void)cameraImageRecordered:(cv::Mat&)img;
@end

@interface Camera : NSObject

- (instancetype)init;

@property (nonatomic, weak) id<CameraDelegate> delegate;

@property (nonatomic, readonly) AVCaptureVideoPreviewLayer *previewLayer;

@end
