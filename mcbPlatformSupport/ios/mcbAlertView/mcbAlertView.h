//
//  mcbAlertView.h
//  
//
//  Created by Radif Sharafullin on 7/5/12.
//  Copyright (c) 2012 Radif Sharafullin. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface mcbAlertView : UIAlertView <UIAlertViewDelegate>
- (id)initWithTitle:(NSString *)title message:(NSString *)message completion:(void (^)(mcbAlertView * alertView, NSInteger buttonIndex))completion cancelButtonTitle:(NSString *)cancelButtonTitle otherButtonTitles:(NSString *)otherButtonTitles, ... NS_REQUIRES_NIL_TERMINATION;
+(void)showAlertWithTitle:(NSString *)title message:(NSString *)message configuration:(void (^)(mcbAlertView * alertView))configuration completion:(void (^)(mcbAlertView * alertView, NSInteger buttonIndex))completion cancelButtonTitle:(NSString *)cancelButtonTitle otherButtonTitles:(NSString *)otherButtonTitles, ... NS_REQUIRES_NIL_TERMINATION;
@end
