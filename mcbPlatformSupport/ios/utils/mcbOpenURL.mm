//
//  mcbOpenURL.m
//  SoundSurfer
//
//  Created by Radif Sharafullin on 12/23/13.
//
//

#include "mcbOpenURL.h"
#import <Foundation/Foundation.h>
#import "mcbAlertView.h"
#import <MessageUI/MessageUI.h>
#import "mcbIOSUtils.h"
#include "mcbPlatformSupport.h"
#include "mcbLogger.h"

@interface mcbMailComposerDelegate : NSObject <MFMailComposeViewControllerDelegate>
+(void)mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError *)error;
@end
@implementation mcbMailComposerDelegate

+(void)mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError *)error{
	switch (result){
		case MFMailComposeResultCancelled:
			break;
		case MFMailComposeResultSaved:
			break;
		case MFMailComposeResultSent:
			break;
		case MFMailComposeResultFailed:
			break;
		default:
			break;
	}
    [controller dismissViewControllerAnimated:TRUE completion:NULL];
}
@end

namespace mcb{namespace PlatformSupport{namespace utils{
    bool canOpenURL(const std::string & urlString){
        NSURL * m_url([NSURL URLWithString:@(urlString.c_str())]);
        return [[UIApplication sharedApplication] canOpenURL:m_url];
    }
    void openURL(const std::string & urlString){
        NSURL * m_url([NSURL URLWithString:@(urlString.c_str())]);
        [[UIApplication sharedApplication] openURL:m_url];
    }
    bool openURL(std::string urlString, std::string alertTitle, std::string alertText, std::string yesButton, std::string noButton){
        NSURL * m_url([NSURL URLWithString:@(urlString.c_str())]);
        if (![[UIApplication sharedApplication] canOpenURL:m_url])
            return false;
        
        [mcbAlertView showAlertWithTitle:@(alertTitle.c_str())
                                 message:@(alertText.c_str())
                           configuration:nil
                              completion:^(mcbAlertView *alertView, NSInteger buttonIndex) {
                                  if (buttonIndex==1)
                                      [[UIApplication sharedApplication] openURL:m_url];
                              }
                       cancelButtonTitle:@(noButton.c_str())
                       otherButtonTitles:@(yesButton.c_str()), nil];
        
        return true;
    }
    void share(const std::string & body, bool addAppIcon, bool addScreenshot){
        
        NSMutableArray * toShare([[@[@(body.c_str())] mutableCopy] autorelease]);
        
        if (addAppIcon)
            [toShare addObject:[UIImage imageNamed:@"icon_152.png"]];
        
        if (addScreenshot) {
            UIImage * imageToSend(iOS::takeScreenshot());
            if (mcb::PlatformSupport::getScreenScaleRatio()>1.f)
                imageToSend=iOS::scaleImageToSize(imageToSend, CGSizeMake(imageToSend.size.width, imageToSend.size.height));
            [toShare addObject:imageToSend];
        }
        
        UIActivityViewController *activityVC = [[UIActivityViewController alloc] initWithActivityItems:toShare applicationActivities:nil];
        activityVC.completionHandler=^(NSString *activityType, BOOL completed){
            std::string activity_type("nil");
            if (activityType)
                activity_type=[activityType UTF8String];
            if (completed)
                Log::mcbLog(PlatformSupport::LogLevelAnalytics, "share", "Shared on activity type \""+activity_type+"\" sheet for: \"" +body+"\"");
            else
                Log::mcbLog(PlatformSupport::LogLevelAnalytics, "share", "Cancelled out of \""+activity_type+"\" share sheet for: \"" +body+"\"");
        };
        activityVC.excludedActivityTypes = @[UIActivityTypePrint, UIActivityTypeCopyToPasteboard, UIActivityTypeAssignToContact, UIActivityTypeSaveToCameraRoll, UIActivityTypeAirDrop];
        [iOS::rootViewController() presentViewController:activityVC animated:YES completion:nil];
        [activityVC release];
        Log::mcbLog(PlatformSupport::LogLevelAnalytics, "share", "Presented share sheet for: \"" +body+"\"");
    }
    void mailto(const std::vector<std::string> & recepients, const std::string & subject, const std::string & body, bool addDeviceInfo){
        if (![MFMailComposeViewController canSendMail]){
            [[[[mcbAlertView alloc] initWithTitle:@"Please, setup email on your device first"
                                                            message:nil
                                                           delegate:nil
                                                  cancelButtonTitle:@"OK"
                                                  otherButtonTitles:nil] autorelease] show];
            return;
        }
        
        
        MFMailComposeViewController *mailComposeController = [[MFMailComposeViewController new] autorelease];
        mailComposeController.mailComposeDelegate = [mcbMailComposerDelegate class];
        
        if(addDeviceInfo){
            NSString* buildVersion = [[[NSBundle mainBundle] infoDictionary] valueForKey:@"CFBundleVersion"];
            [mailComposeController setSubject:@((subject+" v"+[buildVersion UTF8String]+" Support Email").c_str())];
        }else
            [mailComposeController setSubject:@(subject.c_str())];
        
        
        NSMutableArray * receipients=[NSMutableArray array];
        
        for (std::string email: recepients)
            [receipients addObject:@(email.c_str())];
        
        
        [mailComposeController setToRecipients:receipients];
        
        if(addDeviceInfo){
            iOS::MemoryInfoInGB memInfo(iOS::memoryInGB());
            
            [mailComposeController setMessageBody:@((body + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"+"Build Version: "+iOS::buildVersion()+"\n"+"Platform: "+iOS::platform()+"\n"+"Locale: "+iOS::locale()+"\n"+"Language: "+iOS::language()+"\n"+"iOSVersion: "+iOS::iOSVersion()+"\n"+"Total Memory: "+memInfo.totalMemory+"GB"+"\n"+"Used Memory: "+memInfo.usedMemory+"GB"+"\n"+"Free Memory: "+memInfo.freeMemory+"GB").c_str()) isHTML:FALSE];
        }else
            [mailComposeController setMessageBody:@(body.c_str()) isHTML:FALSE];
        
        
        [iOS::rootViewController() presentViewController:mailComposeController animated:TRUE completion:NULL];
    }

}}}

