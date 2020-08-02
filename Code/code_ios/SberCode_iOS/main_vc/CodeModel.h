//
//  CodeModel.h
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 02.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CodeModel : NSObject
@property (nonatomic, strong) NSString *typeName;
@property (nonatomic, strong) NSString *message;
@property (nonatomic, assign, readonly) BOOL _2dCode;
@end

NS_ASSUME_NONNULL_END
