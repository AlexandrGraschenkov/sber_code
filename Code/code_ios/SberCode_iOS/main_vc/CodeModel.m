//
//  CodeModel.m
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 02.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import "CodeModel.h"

@implementation CodeModel
- (BOOL)isEqual:(id)other
{
    if (other == self) {
        return YES;
    } else if ([other isKindOfClass:[CodeModel class]]) {
        return [self.typeName isEqual:[other typeName]] &&
                [self.message isEqual:[other message]];
    }
    return NO;
}

- (NSUInteger)hash
{
    return self.typeName.hash + self.message.hash;
}

- (BOOL)_2dCode {
    NSArray *codes = @[@"QR-Code", @"QR_CODE", @"AZTEC", @"DATA_MATRIX"];
    if ([codes containsObject:self.typeName]) {
        return true;
    } else {
        return false;
    }
}
@end
