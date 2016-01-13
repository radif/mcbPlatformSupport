//
//  PlatformSupport.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#include "mcbPlatformSupport.h"
#import <Foundation/Foundation.h>

namespace mcb{namespace PlatformSupport{
    std::string _platformSpecificDocsPath(){
        static std::string retVal([[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String]);
        return retVal;
    }
    std::string _platformSpecificCachesPath(){
        static std::string retVal([[NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String]);
        return retVal;
    }
    std::string _platformSpecificLibPath(){
        static std::string retVal([[NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String]);
        return retVal;
    }
    std::string _platformSpecificBundlePath(){
        static std::string retVal([[[NSBundle mainBundle] resourcePath] UTF8String]);
        return retVal;
    }
    bool isTVPlatform(){
#if TARGET_OS_IOS
        // iOS-specific code
        return false;
#elif TARGET_OS_TV
        // tvOS-specific code
        return true;
#endif
    }
}}