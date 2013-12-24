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
           
            iOS::MemoryInfoInMB memInfo(iOS::memoryInMB());
            
            [mailComposeController setMessageBody:@((body + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"+"Build Version: "+iOS::buildVersion()+"\n"+"Platform: "+iOS::platform()+"\n"+"Locale: "+iOS::locale()+"\n"+"Language: "+iOS::language()+"\n"+"iOSVersion: "+iOS::iOSVersion()+"\n"+"Total Memory: "+memInfo.totalMemory+"MB"+"\n"+"Used Memory: "+memInfo.usedMemory+"MB"+"\n"+"Free Memory: "+memInfo.freeMemory+"MB").c_str()) isHTML:FALSE];
        }else
            [mailComposeController setMessageBody:@(body.c_str()) isHTML:FALSE];
        
        
        [iOS::rootViewControler() presentViewController:mailComposeController animated:TRUE completion:NULL];
    }

}}}

