//
//  Constants.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#ifndef __mcb__PlatformSupportConstants__
#define __mcb__PlatformSupportConstants__

#include <iostream>
namespace mcb{ namespace PlatformSupport{namespace Constants{

    //device idiom
    extern const char * kDeviceIdiomiPhone;
    extern const char * kDeviceIdiomiPad;
    extern const char * kDeviceIdiomiPhoneTall;
    
    
    //positions
    extern const char * kRelativePointOnScreen;
    extern const char * kAbsoluteRelativePointOnScreen;
    extern const char * kRelativePositionFromAbsoluteString;
    extern const char * kRelativePositionFromCenter;

    extern const char * kCenterPositionString;
    extern const char * kTopLeftPositionString;
    extern const char * kTopRightPositionString;
    extern const char * kBottomLeftPositionString;
    extern const char * kBottomRightPositionString;

    //files
    extern const char * kManifestDataFile;
    
    //misc
    extern const char * kContextString;

}}}
#endif /* defined(__mcb__PlatformSupportConstants__) */
