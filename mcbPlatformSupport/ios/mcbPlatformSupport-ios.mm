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
    NSString *_docPath(){
        return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    }
    
    NSString *_cacPath(){
        return [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    }
    
    NSString *_libPath(){
        return [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    }
    NSString * _mcbPath(NSString *inPath) {

        //global
        if ([inPath rangeOfString:@"$(SHARED)"].location!=NSNotFound)
            inPath=[inPath stringByReplacingOccurrencesOfString:@"$(SHARED)" withString:@(getSharedBundlePath().c_str())];
        
        // bundle
        if ([inPath rangeOfString:@"$(BUNDLE)"].location!=NSNotFound)
            inPath=[inPath stringByReplacingOccurrencesOfString:@"$(BUNDLE)" withString:[[NSBundle mainBundle] resourcePath]];
        //documents
        if ([inPath rangeOfString:@"$(DOCUMENTS)"].location!=NSNotFound)
            inPath=[inPath stringByReplacingOccurrencesOfString:@"$(DOCUMENTS)" withString:_docPath() ];
        //cache
        if ([inPath rangeOfString:@"$(CACHES)"].location!=NSNotFound)
            inPath=[inPath stringByReplacingOccurrencesOfString:@"$(CACHES)" withString:_cacPath()];
        //Library
        if ([inPath rangeOfString:@"$(LIBRARY)"].location!=NSNotFound)
            inPath=[inPath stringByReplacingOccurrencesOfString:@"$(LIBRARY)" withString:_libPath()];
        //General Assets
        if ([inPath rangeOfString:@"$(ASSETS)"].location!=NSNotFound)
            inPath=[inPath stringByReplacingOccurrencesOfString:@"$(ASSETS)" withString:[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"assets"]];
        //Downloads
//        if ([inPath rangeOfString:@"$(DOWNLOADS)"].location!=NSNotFound)
//            inPath=[inPath stringByReplacingOccurrencesOfString:@"$(DOWNLOADS)" withString:[_libPath() stringByAppendingPathComponent:downloadsStr]];
        
        return inPath;
    }
    
    NSString * _mcbPathForLocalDirectory(NSString *inPath, NSString *localDirectory){
        inPath= _mcbPath(inPath);
        if ([inPath rangeOfString:@"$(LOCAL).."].location!=NSNotFound)
            inPath=[inPath stringByReplacingOccurrencesOfString:@"$(LOCAL).." withString:[localDirectory stringByDeletingLastPathComponent]];
        if ([inPath rangeOfString:@"$(LOCAL)"].location!=NSNotFound)
            inPath=[inPath stringByReplacingOccurrencesOfString:@"$(LOCAL)" withString:localDirectory ];
        return  inPath;
    }
    
    
    std::string resolvePath(const std::string & inPath, const std::string & localDirectory){
        if (localDirectory.empty())
            return [_mcbPath(@(inPath.c_str())) UTF8String];
        return [_mcbPathForLocalDirectory(@(inPath.c_str()), @(localDirectory.c_str())) UTF8String];
    }
    
        
           
}}