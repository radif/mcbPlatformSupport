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
    
    pAudioPlayer AudioCache::loadSound(const std::string & path, const std::string & key){
        std::string pathOrKey(key.empty()? path : key);
        
        pAudioPlayer retVal(nullptr);
        
        auto it(_players.find(pathOrKey));
        if (it==_players.end()){
            retVal=std::make_shared<AudioPlayer>(path);
            _players[pathOrKey]={retVal,1};//initial retain count is 1
        }else{
            it->second.second++;//bump up the retain count
            retVal=it->second.first;
        }
        
        return retVal;
    }
    
    pAudioPlayer AudioCache::playerForSound(const std::string & pathOrKey) const{
        auto it(_players.find(pathOrKey));
        if (it==_players.end())
            return nullptr;
        return it->second.first;
    }
    
    pAudioPlayer AudioCache::playAudioSound(const std::string & pathOrKey){
        pAudioPlayer player(playerForSound(pathOrKey));
        if (!player)
            player=loadSound(pathOrKey);
        player->play();
        return player;
    }
    pAudioPlayer AudioCache::stopAudioSound(const std::string & pathOrKey){
        pAudioPlayer player(playerForSound(pathOrKey));
        if (!player)
            player=loadSound(pathOrKey);
        player->stop();
        return player;
    }
    
    void AudioCache::iterateThroughSounds(std::function<void(const std::string & pathOrKey, pAudioPlayer player)> handle) const{
        if (!handle)
            return;
        for (const auto & pair : _players)
            handle(pair.first, pair.second.first);
    }
    
    void AudioCache::unloadSound(const std::string & pathOrKey){
        auto it(_players.find(pathOrKey));
        if (it==_players.end())
            return;//failing silently
        
        if (it->second.second<=1)
            _players.erase(it);
        else
            it->second.second--;
        
    }
    void AudioCache::drain(){
        _players.erase(_players.begin(), _players.end());
    }
    
}}
