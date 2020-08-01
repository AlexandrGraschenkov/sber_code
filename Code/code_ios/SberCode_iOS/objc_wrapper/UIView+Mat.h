//
//  UIView+Mat.h
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 01.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <opencv2/core.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface UIView (Mat)

- (void)displayContentMat:(cv::Mat)mat;

@end

NS_ASSUME_NONNULL_END
