//
//  VisualizeBehavior.h
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 01.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#import <sber_code/sber_code.h>

NS_ASSUME_NONNULL_BEGIN

@interface VisualizeBehavior : NSObject
//- (instancetype)initWithView:(UIView *)displayView;
- (instancetype)initWithLayer:(AVCaptureVideoPreviewLayer *)displayLayer;

- (void)updateRecognitions:(const std::vector<SberCode::Code> &)recogVec size:(cv::Size)imageSize;
- (void)reset;
@end

NS_ASSUME_NONNULL_END
