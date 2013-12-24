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
#include <sys/types.h>
#include <sys/sysctl.h>

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
    
    UIViewController * _rootViewControler(){
        UIViewController *rootViewController = nil;
        id appDelegate = [[UIApplication sharedApplication] delegate];
        if ([appDelegate respondsToSelector:@selector(viewController)])
        {
            rootViewController = [appDelegate valueForKey:@"viewController"];
        }
        if (!rootViewController && [appDelegate respondsToSelector:@selector(window)])
        {
            UIWindow *window = [appDelegate valueForKey:@"window"];
            rootViewController = window.rootViewController;
        }
        return rootViewController;
    }
    
    NSString * _platform(){
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char *machine = (char *)malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        NSString *platform = [NSString stringWithUTF8String:machine];
        free(machine);
        return platform;
    }
    
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
            NSString* buildVersion = [[[NSBundle mainBundle] infoDictionary] valueForKey:@"CFBundleVersion"];
            NSString* locale = [[NSLocale currentLocale] localeIdentifier];
            NSString* language = [[NSLocale preferredLanguages] objectAtIndex:0];
            NSString* iOSVersion = [[UIDevice currentDevice] systemVersion];
            float totalSpace = 0.0f;
            float freeSpace = 0.0f;
            float usedSpace = 0.0f;
            
            NSError *error = nil;
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            NSDictionary *dictionary = [[NSFileManager defaultManager] attributesOfFileSystemForPath:[paths lastObject] error: &error];
            
            if (dictionary) {
                NSNumber* fileSystemSizeInBytes = [dictionary objectForKey: NSFileSystemSize];
                NSNumber* fileSystemFreeSizeInBytes = [dictionary objectForKey: NSFileSystemFreeSize];
                
                totalSpace = [fileSystemSizeInBytes floatValue]/1024/1024/1024;
                freeSpace = [fileSystemFreeSizeInBytes floatValue]/1024/1024/1024;
                usedSpace = ([fileSystemSizeInBytes floatValue] - [fileSystemFreeSizeInBytes floatValue])/1024/1024/1024;
            }
            NSString* memCapacity = [NSString stringWithFormat:@"%ld", lroundf(totalSpace)];
            NSString* memUsed = [NSString stringWithFormat:@"%ld", lroundf(usedSpace)];
            NSString* memFree = [NSString stringWithFormat:@"%ld", lroundf(freeSpace)];
            
            [mailComposeController setMessageBody:@((body + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"+"Build Version: "+[buildVersion UTF8String]+"\n"+"Locale: "+[locale UTF8String]+"\n"+"Language: "+[language UTF8String]+"\n"+"iOSVersion: "+[iOSVersion UTF8String]+"\n"+"Total Memory: "+[memCapacity UTF8String]+"MB"+"\n"+"Used Memory: "+[memUsed UTF8String]+"MB"+"\n"+"Free Memory: "+[memFree UTF8String]+"MB").c_str()) isHTML:FALSE];
        }else
            [mailComposeController setMessageBody:@(body.c_str()) isHTML:FALSE];
        
        
        [_rootViewControler() presentViewController:mailComposeController animated:TRUE completion:NULL];
    }

}}}

