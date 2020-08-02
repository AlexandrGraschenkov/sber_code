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
#include "CodeModel.h"
#include "CodeCell.h"


@interface ViewController () <CameraDelegate, UITableViewDelegate, UITableViewDataSource> {
    SberCode::Recognizer recognizer;
    Camera *cam;
    FPS *fps;
    FPSIteration *fpsIter;
    VisualizeBehavior *visBehavior;
    NSMutableArray *foundedCodes;
}
@property (nonatomic, weak) IBOutlet UILabel *fpsLabel;
@property (nonatomic, weak) IBOutlet UIView *debugView;
@property (nonatomic, weak) IBOutlet UITableView *table;
@property (atomic, assign) BOOL showDebug;

@end

@implementation ViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

    foundedCodes = [NSMutableArray new];
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
    self.table.contentInset = UIEdgeInsetsMake(self.table.frame.size.height - 100, 0, 0, 0);
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
        self.fpsLabel.text = [NSString stringWithFormat:@"%.2lf < FPS\t%.2lf <Process", fpsVal, fpsProcess];
        [self->visBehavior updateRecognitions:recognized size:imgSize];
        [self updateTableNewCodes:recognized];
    });
}

- (void)updateTableNewCodes:(const std::vector<SberCode::Code> &)codes {
    if (codes.size() == 0) return;
    BOOL changes = false;
    
    for (const SberCode::Code &c : codes) {
        CodeModel *code = [CodeModel new];
        code.typeName = [NSString stringWithUTF8String:c.typeName.c_str()];
        code.message = [NSString stringWithUTF8String:c.message.c_str()];
        
        NSUInteger idx = [foundedCodes indexOfObject:code];
        if (idx == 0) {
            // everything fine
            continue;
        }
        
        if (idx != NSNotFound) {
            [foundedCodes removeObject:code];
        }
        
        [foundedCodes insertObject:code atIndex:0];
        changes = true;
    }
    
    if (changes) {
        [self.table reloadData];
    }
}

// MARK: - Table

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return foundedCodes.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    CodeCell *cell = [tableView dequeueReusableCellWithIdentifier:@"code" forIndexPath:indexPath];
    if (cell == nil) {
        return [UITableViewCell new];
    }
    
    CodeModel *m = foundedCodes[indexPath.row];
    cell.titleLabel.text = m.message;
    cell.detailLabel.text = m.typeName;
    cell.icon.image = m._2dCode ? [UIImage imageNamed:@"2d_code"] : [UIImage imageNamed:@"1d_code"];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    CodeModel *m = foundedCodes[indexPath.row];
    [UIPasteboard generalPasteboard].string = m.message;
    
    // display alert
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Скопировано в буффер обмена!" message:nil preferredStyle:UIAlertControllerStyleAlert];
    [self presentViewController:alert animated:YES completion:nil];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [alert dismissViewControllerAnimated:YES completion:^{
            [tableView deselectRowAtIndexPath:indexPath animated:YES];
        }];
    });
}
@end
