//
//  mcbSoundPicker.h
//  mcb
//
//  Created by Radif Sharafullin on 8/4/13.
//
//

#ifndef __mcb__SoundPicker__
#define __mcb__SoundPicker__

#include <iostream>
namespace mcb{namespace PlatformSupport{namespace SoundPicker{
    void pickItemFromMusicLibrary(std::function<void(const std::string & itemFileCopyPath)> completion);
    std::string localPlaybackPath();
}}}

#endif /* defined(__mcb__SoundPicker__) */
