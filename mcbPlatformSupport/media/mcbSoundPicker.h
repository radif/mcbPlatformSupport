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
namespace cocos2d{
    class CCTexture2D;
}
namespace mcb{namespace PlatformSupport{namespace SoundPicker{
    void pickItemFromMusicLibrary(std::function<void(const std::string & itemFileCopyPath)> completion);
    std::string localPlaybackPath();
    void metadataForMediaFile(const std::string & filePath, const std::function<void(cocos2d::CCTexture2D * tex, const::std::string & songTitle, const::std::string & albumTitle)>);
}}}

#endif /* defined(__mcb__SoundPicker__) */
