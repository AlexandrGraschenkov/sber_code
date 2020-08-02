//
//  VisualizeBehavior.m
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 01.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import "VisualizeBehavior.h"
#import "DisplayTrackPoly.h"

@interface VisualizeBehavior () {
    NSMutableArray<DisplayTrackPoly *> *polygons;
    AVCaptureVideoPreviewLayer *layer;
}
@end

@implementation VisualizeBehavior
- (instancetype)initWithLayer:(AVCaptureVideoPreviewLayer *)displayLayer {
    self = [super init];
    if (self) {
        layer = displayLayer;
        polygons = [NSMutableArray new];
    }
    return self;
}

- (void)convertPoints:(std::vector<cv::Point> &)points imageSize:(cv::Size)size {
    for (int i = 0; i < points.size(); i++) {
        CGPoint p = CGPointMake(points[i].y / (float)size.height,
                                1 - points[i].x / (float)size.width);
        
//        CGPoint p = CGPointMake(points[i].x / (float)size.width,
//                                points[i].y / (float)size.height);
        
        p = [layer pointForCaptureDevicePointOfInterest:p];
        points[i].x = p.x;
        points[i].y = p.y;
    }
}

- (void)updateRecognitions:(const std::vector<SberCode::Code> &)recogVec size:(cv::Size)imageSize {
    NSMutableArray<NSNumber *> *updatedPolygons = [NSMutableArray new];
    
    double currTime = CACurrentMediaTime();
    for (const SberCode::Code &c : recogVec) {
        std::vector<cv::Point> points = c.location;
        [self convertPoints:points imageSize:imageSize];
        
        bool founded = false;
        for (int polyIdx = 0; polyIdx < polygons.count; polyIdx++) {
            DisplayTrackPoly *poly = polygons[polyIdx];
            if (poly.trackId == c.trackId) {
                founded = true;
                bool animate = (poly.missCount < 30);
                poly.missCount = 0;
                
                [poly updatePoints:points animated:animate];
                poly.lastUpdateTime = currTime;
                if (poly.opacity != 1) poly.opacity = 1;
                [updatedPolygons addObject:@(polyIdx)];
            }
        }
        
        if (!founded) {
            // полигона еще нет, создаем
            DisplayTrackPoly *poly = [DisplayTrackPoly new];
            poly.frame = layer.bounds;
            [poly updatePoints:points animated:false];
            poly.lastUpdateTime = currTime;
            poly.trackId = c.trackId;
            poly.missCount = 0;
            [polygons addObject:poly];
            [layer addSublayer:poly];
        }
    }
    
    // удаляем треки, которые мы давно не видели
    for (int polyIdx = (int)polygons.count-1; polyIdx >= 0; polyIdx--) {
        if ([updatedPolygons containsObject:@(polyIdx)]) continue;
        DisplayTrackPoly *poly = polygons[polyIdx];
        poly.missCount++;
        if (poly.missCount > 21 && poly.opacity != 0) poly.opacity = 0;
        
        if (abs(poly.lastUpdateTime - currTime) > 5.0) {
            [poly removeFromSuperlayer];
            [polygons removeObjectAtIndex:polyIdx];
        }
    }
}

- (void)reset {
    for (DisplayTrackPoly *poly in polygons) {
        [poly removeFromSuperlayer];
    }
    [polygons removeAllObjects];
}
@end
