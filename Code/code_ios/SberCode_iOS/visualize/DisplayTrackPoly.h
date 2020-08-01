//
//  DisplayTrackPoly.h
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 01.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#include <opencv2/core.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface DisplayTrackPoly : CAShapeLayer
@property (nonatomic, assign) int trackId;
@property (nonatomic, assign) int missCount;
@property (nonatomic, assign) double lastUpdateTime;
- (void)updatePoints:(std::vector<cv::Point>)points animated:(BOOL)animated;
@end

NS_ASSUME_NONNULL_END
