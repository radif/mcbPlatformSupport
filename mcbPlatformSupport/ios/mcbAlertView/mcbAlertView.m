//
//  mcbAlertView.m
//  
//
//  Created by Radif Sharafullin on 7/5/12.
//  Copyright (c) 2012 Radif Sharafullin. All rights reserved.
//

#import "mcbAlertView.h"

#if !__has_feature(objc_arc)
#error THIS CODE MUST BE COMPILED WITH ARC ENABLED!
#endif

@interface mcbAlertView ()
@property (copy, nonatomic) void (^completion)(mcbAlertView * alertView, NSInteger buttonIndex);
@end
@implementation mcbAlertView
- (id)initWithTitle:(NSString *)title message:(NSString *)message completion:(void (^)(mcbAlertView * alertView, NSInteger buttonIndex))completion cancelButtonTitle:(NSString *)cancelButtonTitle otherButtonTitles:(NSString *)otherButtonTitles, ... {
    
    self.completion = completion;
    return [super initWithTitle:title message:message delegate:self cancelButtonTitle:cancelButtonTitle otherButtonTitles:otherButtonTitles, nil];
}
+(void)showAlertWithTitle:(NSString *)title message:(NSString *)message configuration:(void (^)(mcbAlertView * alertView))configuration completion:(void (^)(mcbAlertView * alertView, NSInteger buttonIndex))completion cancelButtonTitle:(NSString *)cancelButtonTitle otherButtonTitles:(NSString *)otherButtonTitles, ... {
    
    mcbAlertView *alert=[[mcbAlertView alloc] initWithTitle:title message:message completion:completion cancelButtonTitle:cancelButtonTitle otherButtonTitles:otherButtonTitles, nil];
    if(configuration)
        configuration(alert);
    [alert show];
    
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex {
    if (self.completion)
        self.completion(self, buttonIndex);
}
@end
