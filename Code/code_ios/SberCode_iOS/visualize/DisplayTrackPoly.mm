//
//  DisplayTrackPoly.m
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 01.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import "DisplayTrackPoly.h"
#import <UIKit/UIKit.h>

@interface DisplayTrackPoly () {
    std::vector<cv::Point> prevPoints;
}

@end

@implementation DisplayTrackPoly
- (instancetype)init
{
    self = [super init];
    if (self) {
        self.strokeColor = [UIColor colorWithRed:0.1 green:0.3 blue:0.9 alpha:1.0].CGColor;
        self.lineWidth = 3;
        self.fillColor = [UIColor clearColor].CGColor;
    }
    return self;
}

- (void)displayPoints:(const std::vector<cv::Point> &)points animated:(BOOL)animated {
    prevPoints = points;
    UIBezierPath *path = [UIBezierPath bezierPath];
    for (int i = 0; i < points.size(); i++) {
        CGPoint p = CGPointMake(points[i].x, points[i].y);
        if (i == 0) [path moveToPoint:p];
        else [path addLineToPoint:p];
    }
    [path closePath];
    
    if (animated) {
        self.path = [path CGPath];
    } else {
        [CATransaction begin];
        [CATransaction setValue: (id) kCFBooleanTrue forKey: kCATransactionDisableActions];
        self.path = [path CGPath];
        [CATransaction commit];
    }
    
}
- (id<CAAction>)actionForKey:(NSString *)event
{
    if ([event isEqualToString:@"path"] || [event isEqualToString:@"opacity"]) {
        CABasicAnimation * strokAnimation = [CABasicAnimation animationWithKeyPath:event];
        strokAnimation.removedOnCompletion = NO;
        strokAnimation.fillMode = kCAFillModeForwards;
        strokAnimation.duration = .25f;
        strokAnimation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseOut];
        strokAnimation.fromValue = [self.presentationLayer valueForKey:event];
        return strokAnimation;
    }
    return [super actionForKey:event];
}


- (void)updatePoints:(std::vector<cv::Point>)points animated:(BOOL)animated {
    if (prevPoints.size() != 0 && points.size() == prevPoints.size()) {
        // проварачиваем точки, чтобы при повороте кммеры код не скакал
        // ищем минимальную дистанцию между точек, при их провороте
        std::vector<double> dists;
        for (int i = 0; i < points.size(); i++) {
            double dist = 0;
            for (int checkIdx = 0; checkIdx < points.size(); checkIdx++) {
                int ii = (checkIdx + i) % points.size();
                cv::Point vec = prevPoints[checkIdx] - points[ii];
                dist += vec.x * vec.x + vec.y * vec.y;
            }
            dists.push_back(dist);
        }
        
        // выбираем наилучший проворот точек
        int rotateIdx = 0;
        for (int i = 1; i < dists.size(); i++) {
            if (dists[rotateIdx] > dists[i]) {
                rotateIdx = i;
            }
        }
        
        // и прокручиваем точки
        while (rotateIdx) {
            cv::Point p = points[0];
            points.erase(points.begin());
            points.push_back(p);
            rotateIdx--;
        }
    }
    
    [self displayPoints:points animated:animated];
}
@end
