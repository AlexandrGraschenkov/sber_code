//
//  CodeCell.h
//  SberCode_iOS
//
//  Created by Alexander Graschenkov on 02.08.2020.
//  Copyright © 2020 Alex the Best. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface CodeCell : UITableViewCell
@property (nonatomic, weak) IBOutlet UILabel *titleLabel;
@property (nonatomic, weak) IBOutlet UILabel *detailLabel;
@property (nonatomic, weak) IBOutlet UIImageView *icon;
@end

NS_ASSUME_NONNULL_END
