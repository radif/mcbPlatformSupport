//
//  mcbOpenURL.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 12/23/13.
//
//

#ifndef __SoundSurfer__mcbOpenURL__
#define __SoundSurfer__mcbOpenURL__

#include <iostream>
#include <vector>
namespace mcb{namespace PlatformSupport{namespace utils{
    bool canOpenURL(const std::string & urlString);
    void openURL(const std::string & urlString);
    bool openURL(std::string urlString, std::string alertTitle, std::string alertText, std::string yesButton, std::string noButton);

    void mailto(const std::vector<std::string> & recepients, const std::string & subject, const std::string & body, bool addDeviceInfo);
}}}
#endif /* defined(__SoundSurfer__mcbOpenURL__) */
