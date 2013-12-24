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
namespace mcb{namespace PlatformSupport{namespace utils{
    bool canOpenURL(const std::string & urlString);
    void openURL(const std::string & urlString);
    bool openURL(std::string urlString, std::string alertTitle, std::string alertText, std::string yesButton, std::string noButton);

}}}
#endif /* defined(__SoundSurfer__mcbOpenURL__) */
