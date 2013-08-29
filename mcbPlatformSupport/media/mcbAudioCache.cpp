//
//  mcbAudioCache.cpp
//  ColorDots
//
//  Created by Radif Sharafullin on 8/29/13.
//
//

#include "mcbAudioCache.h"
#include <mutex>

namespace mcb{namespace PlatformSupport{
    static AudioCache * _sharedAudioCache=nullptr;
    AudioCache* AudioCache::sharedInstance(){
        static std::once_flag onceFlag;
        std::call_once(onceFlag,[](){
            if (!_sharedAudioCache)
                _sharedAudioCache=new AudioCache;
        });
        return _sharedAudioCache;
    }
    
    
    void AudioCache::loadSound(const std::string & path, const std::string & key){
        std::string pathOrKey(key.empty()? path : key);
        pAudioPlayer player(playerForSound(pathOrKey));
        if(!player)
            _players[pathOrKey]=std::make_shared<AudioPlayer>(path);
    }
    
    pAudioPlayer AudioCache::playerForSound(const std::string & pathOrKey){
        auto it(_players.find(pathOrKey));
        if (it==_players.end())
            return nullptr;
        return (*it).second;
    }
    
    pAudioPlayer AudioCache::playAudioSound(const std::string & pathOrKey){
        pAudioPlayer player(playerForSound(pathOrKey));
        player->play();
        return player;
    }
    pAudioPlayer AudioCache::stopAudioSound(const std::string & pathOrKey){
        pAudioPlayer player(playerForSound(pathOrKey));
        player->stop();
        return player;
    }
    
    void AudioCache::iterateThroughSounds(std::function<void(const std::string & pathOrKey, pAudioPlayer player)> handle){
        if (!handle)
            return;
        for (const auto & pair : _players)
            handle(pair.first, pair.second);
    }
    
    void AudioCache::unloadSound(const std::string & pathOrKey){
        auto it(_players.find(pathOrKey));
        if (it==_players.end())
            return;//failing silently
        _players.erase(it);
    }
    void AudioCache::unloadAllSounds(){
        _players.erase(_players.begin(), _players.end());
    }
    
}}
