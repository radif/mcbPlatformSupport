//
//  mcbSoundPicker.cpp
//  mcb
//
//  Created by Radif Sharafullin on 8/4/13.
//
//

#include "mcbSoundPicker.h"
#include "mcbPlatformSupport.h"

namespace mcb{namespace PlatformSupport{namespace SoundPicker{
    const char * kMediaLibraryUpdatedNotification="kMediaLibraryUpdatedNotification";

    std::string localPlaybackPath(){
        return PlatformSupport::resolvePath("$(LIBRARY)/music.mp4");
    }
    
   /*
    void pickItemFromMusicLibrary(std::function<void(const std::string & itemFileCopyPath)> completion){
    
        
    }
    */
    
}}}