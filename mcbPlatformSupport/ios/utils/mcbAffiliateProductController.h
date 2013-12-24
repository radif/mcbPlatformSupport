//
//  mcbAffiliateProductController.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 12/24/13.
//
//

#import "mcbIOSUtils.h"

@interface mcbAffiliateProductController : NSObject
-(instancetype)initWithProductId:(NSString *)productId callbackUrl:(NSString *)callbackUrl;
+(mcbAffiliateProductController *)createWithProductId:(NSString *)productId callbackUrl:(NSString *)callbackUrl;
+(BOOL)isAvailable;
-(void)showInViewController:(UIViewController *)viewController completion:(MCBGeneralPurposeBlock)completion;
@end
