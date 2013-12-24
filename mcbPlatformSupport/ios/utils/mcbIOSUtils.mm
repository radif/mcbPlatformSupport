//
//  mcbIOSUtils.m
//  SoundSurfer
//
//  Created by Radif Sharafullin on 12/24/13.
//
//

#import "mcbIOSUtils.h"
#include <sys/types.h>
#include <sys/sysctl.h>
namespace mcb{namespace PlatformSupport{namespace utils{namespace iOS{
    UIViewController * rootViewControler(){
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
    std::string platform(){
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char *machine = (char *)malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        NSString *platform = [NSString stringWithUTF8String:machine];
        free(machine);
        return [platform UTF8String];
    }
    std::string buildVersion(){
        return [[[[NSBundle mainBundle] infoDictionary] valueForKey:@"CFBundleVersion"] UTF8String];
    }
    std::string locale(){
        return [[[NSLocale currentLocale] localeIdentifier] UTF8String];
    }
    std::string language(){
        return [[NSLocale preferredLanguages][0] UTF8String];
    }
    std::string iOSVersion(){
        return [[[UIDevice currentDevice] systemVersion] UTF8String];
    }
    MemoryInfoInGB memoryInGB(){
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
        return MemoryInfoInGB{[memCapacity UTF8String], [memUsed UTF8String], [memFree UTF8String]};
    }
}}}}
