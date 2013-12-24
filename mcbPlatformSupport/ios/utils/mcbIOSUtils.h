//
//  mcbIOSUtils.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 12/24/13.
//
//

#import <Foundation/Foundation.h>
#include <iostream>

#define mcbClassLog(format, ...) NSLog(@((std::string("%@: ")+[format UTF8String]).c_str()), NSStringFromClass(self.class), ##__VA_ARGS__)

typedef void (^MCBGeneralPurposeBlock)();


namespace mcb{namespace PlatformSupport{namespace utils{namespace iOS{
    UIViewController * rootViewControler();
    
    std::string platform();
    std::string buildVersion();
    std::string locale();
    std::string language();
    std::string iOSVersion();
    
    struct MemoryInfoInGB{std::string totalMemory, usedMemory, freeMemory;};
    MemoryInfoInGB memoryInGB();
    
}}}}