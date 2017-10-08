//
//  Constants.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#include "mcbPlatformSupportConstants.h"

namespace mcb{ namespace PlatformSupport{namespace Constants{
    //device idiom
    const char * kDeviceIdiomiPhone("kDeviceIdiomiPhone");
    const char * kDeviceIdiomiPad("kDeviceIdiomiPad");
    const char * kDeviceIdiomiPhoneTall("kDeviceIdiomiPhoneTall");
    
    //positions
    const char * kRelativePointOnScreen("r{");
    const char * kEdgeProtectedRelativePointOnScreen("e{");
    const char * kRelativePositionFromAbsoluteString("rpos{");
    const char * kRelativePositionFromCenter("c{");

    const char * kCenterPositionString("mcbCenterPositionString");
    const char * kTopLeftPositionString("mcbTopLeftPositionString");
    const char * kTopRightPositionString("mcbTopRightPositionString");
    const char * kBottomLeftPositionString("mcbBottomLeftPositionString");
    const char * kBottomRightPositionString("mcbBottomRightPositionString");
    
    //files
    const char * kManifestDataFile("manifest.data");
    
    //misc
    const char * kContextString("context");
}}}
