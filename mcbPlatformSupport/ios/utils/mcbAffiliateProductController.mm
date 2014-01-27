//
//  mcbAffiliateProductController.m
//  SoundSurfer
//
//  Created by Radif Sharafullin on 12/24/13.
//
//

#import "mcbAffiliateProductController.h"
#import <StoreKit/StoreKit.h>
#import "AFNetworking.h"

#if !__has_feature(objc_arc)
#error THIS CODE MUST BE COMPILED WITH ARC ENABLED!
#endif

@interface mcbAffiliateProductController() <SKStoreProductViewControllerDelegate>
@end
@implementation mcbAffiliateProductController{
    __strong NSString *_productId, *_callbackUrl;
    __strong MCBGeneralPurposeBlock _completion;
    __strong mcbAffiliateProductController *_strongSelf;
    __weak UIActivityIndicatorView * _spinner;
}
+(BOOL)isAvailable{
    return [SKStoreProductViewController class]!=Nil;
}
-(instancetype)initWithProductId:(NSString *)product callbackUrl:(NSString *)callback {
    if(self=[super init]){
        _productId = product;
        _callbackUrl = callback;
        _completion=NULL;
        _strongSelf=nil;
    }
    return self;
}

+(mcbAffiliateProductController *)createWithProductId:(NSString *)productId callbackUrl:(NSString *)callbackUrl {
    return [[mcbAffiliateProductController alloc] initWithProductId:productId callbackUrl:callbackUrl];
}
- (void)showInViewController:(UIViewController *)viewController completion:(MCBGeneralPurposeBlock)completion{
    BOOL available = self.class.isAvailable;
    _completion=[completion copy];
    _strongSelf=self;
    __block NSURL *lastUrl;
    
    // Execute the callback and follow all redirects
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:_callbackUrl]];
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    [operation setRedirectResponseBlock:^NSURLRequest *(NSURLConnection *connection, NSURLRequest *request, NSURLResponse *redirectResponse) {
        lastUrl = request.URL;
        if (!redirectResponse)
            mcbClassLog(@"Callback started: %@", lastUrl.absoluteString);
        else
            mcbClassLog(@"Callback redirected to: %@", lastUrl.absoluteString);
        return request;
    }];
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        if (available)
            mcbClassLog(@"Callback finished");
        else{
            if ([lastUrl.host isEqualToString:@"itunes.apple.com"]) {
                mcbClassLog(@"Opening iTunes URL externally");
                [[UIApplication sharedApplication] openURL:lastUrl];
            }else
                mcbClassLog(@"Callback didn't lead to iTunes URL");
        }
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        mcbClassLog(@"Callback failed. %@",error);
    }];
    [operation start];
    
    if (available) {
        // Prepare the product view controller by providing the product ID.
        SKStoreProductViewController *productViewController([SKStoreProductViewController new]);
        productViewController.delegate=self;
        NSDictionary *storeParameters(@{SKStoreProductParameterITunesItemIdentifier: _productId});
        UIActivityIndicatorView *spinner([[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle: UIActivityIndicatorViewStyleGray]);
        _spinner=spinner;
        __weak SKStoreProductViewController * weakProductViewController(productViewController);
        __weak UIActivityIndicatorView * weakSpinner(spinner);
        // Present the product view controller
        [viewController presentViewController:productViewController animated:YES completion:^(void) {
            weakSpinner.hidesWhenStopped = YES;
            if (weakSpinner)
                [weakProductViewController.view addSubview:weakSpinner];
            CGSize size = productViewController.view.bounds.size;
            [weakSpinner setCenter:CGPointMake(size.width/2, size.height/2)];
            [weakSpinner startAnimating];
            mcbClassLog(@"Presented product view controller");
        }];
        
        // Try to load the product and dismiss the product view controller in case of failure
        [productViewController loadProductWithParameters:storeParameters completionBlock:^(BOOL result, NSError *error) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [spinner stopAnimating];
                [spinner removeFromSuperview];
            });
            if (result)
                mcbClassLog(@"Presented product: %@", _productId);
            else
                mcbClassLog(@"Failed to load product: %@", error);
        }];
    }
}

// SKStoreProductViewControllerDelegate method that dismisses the product view controller
- (void)productViewControllerDidFinish:(SKStoreProductViewController *)viewController {
    [viewController dismissViewControllerAnimated:YES completion:^{
        if (_completion)
            _completion();
        _strongSelf=nil;
    }];
}
-(void)dealloc{
    if (_spinner) {
        [_spinner stopAnimating];
        [_spinner removeFromSuperview];
    }
}
@end
