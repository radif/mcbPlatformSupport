//
//  mcbIOSUtils.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 12/24/13.
//
//

#import <Foundation/Foundation.h>
#include <iostream>
namespace mcb{namespace PlatformSupport{namespace utils{namespace iOS{
    UIViewController * rootViewControler();
    
    std::string platform();
    std::string buildVersion();
    std::string locale();
    std::string language();
    std::string iOSVersion();
    
    struct MemoryInfoInMB{std::string totalMemory, usedMemory, freeMemory;};
    MemoryInfoInMB memoryInMB();
    
}}}}