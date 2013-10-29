//
//  mcbAudioCache.h
//  ColorDots
//
//  Created by Radif Sharafullin on 8/29/13.
//
//

#ifndef __ColorDots__mcbAudioCache__
#define __ColorDots__mcbAudioCache__

#include "mcbAudioPlayer.h"
#include <map>
#include <mcbFactory.h>

namespace mcb{namespace PlatformSupport{
    class AudioCache : public PlatformSupport::SingletonFactory<AudioCache>{
        friend PlatformSupport::SingletonFactory<AudioCache>;
        std::map<std::string, std::pair<pAudioPlayer, int>> _players;//retian counted
        virtual void init() override{}
    public:
        
        //loads the sound or increments ref count if already loaded
        pAudioPlayer loadSound(const std::string & path, const std::string & key="");
        pAudioPlayer playerForSound(const std::string & pathOrKey) const;
        
        //plays the sound or loads and plays if not loaded previously, subject to unload the sound once used without load
        pAudioPlayer playAudioSound(const std::string & pathOrKey);
        
        //stops the sound or loads and stops if not loaded previously, subject to unload the sound once used without load
        pAudioPlayer stopAudioSound(const std::string & pathOrKey);
        
        //basic iteration
        void iterateThroughSounds(std::function<void(const std::string & pathOrKey, pAudioPlayer player)> handle) const;

        //unloads the sound or decrements ref count if it is more than 1
        void unloadSound(const std::string & pathOrKey);
        
        //clears all the cached sounds regardless of the ref count
        void drain();
    };
}}

#endif /* defined(__ColorDots__mcbAudioCache__) */
