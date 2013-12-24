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
}}}

