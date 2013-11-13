//
//  PlaylistPlayer.cpp
//  mcb
//
//  Created by Radif Sharafullin on 7/01/13.
//
//

#include "mcbPlaylistPlayer.h"
namespace mcb{namespace PlatformSupport{
    PlaylistPlayer::PlaylistPlayer(const std::string & playlistName, const std::vector<std::string> & audioFiles): _playlistName(playlistName), _audioFiles(audioFiles){
       
    }
    PlaylistPlayer::~PlaylistPlayer(){
        onPlaylistCompletedHandle=nullptr;
        stop();
    }
    
    void PlaylistPlayer::_playNextItemInQueue(){
        
        if (_audioItemIndex>=_audioFiles.size()) {
            stop();
            return;
        }else{
            _player= std::make_shared<AudioPlayer>("tts", _audioFiles.at(_audioItemIndex), 0, 1, 0);
            _player->setMeteringEnabled(_isMeteringEnabled);
            _player->prepareToPlay();
            _player->m_playerCompletion=[=](AudioPlayer * p){_playNextItemInQueue();};
            _player->play();
        }
        _audioItemIndex++;
    }
    
    
    void PlaylistPlayer::play(){
        stop();
        _playNextItemInQueue();
    }
    void PlaylistPlayer::pause(){
        if (_player)
            _player->pause();
    }
    void PlaylistPlayer::stop(){
        if (_player){
            _player->m_playerCompletion=nullptr;
            _player->m_playerFinishedHandle=nullptr;
            _player=nullptr;
            _audioItemIndex=0;
            if (onPlaylistCompletedHandle)
                onPlaylistCompletedHandle(this, _playlistName);
        }
    }
    
    
    /* properties */
    bool PlaylistPlayer::getIsPlaying() const{
        if (_player)
            return _player->getIsPlaying();
        return false;
    }
    
    void PlaylistPlayer::setMeteringEnabled(bool enabled){
        _isMeteringEnabled=enabled;
        if (_player)
            _player->setMeteringEnabled(_isMeteringEnabled);
        
    }
    bool PlaylistPlayer::isMeteringEnabled(){
        return _isMeteringEnabled;
    }
    
    void PlaylistPlayer::updateMeters(){
        if (_player)
            _player->updateMeters();
    }
    
    float PlaylistPlayer::getPeakPowerForChannel(int channelNumber){
        if (_player)
            return _player->getPeakPowerForChannel(channelNumber);
        return 0.f;
    }
    float PlaylistPlayer::getAveragePowerForChannel(int channelNumber){
        if (_player)
            return _player->getAveragePowerForChannel(channelNumber);
        return 0.f;
    }
    
    void PlaylistPlayer::setSuspended(bool suspended){
        if (_player)
            _player->setSuspended(suspended);
    }
}}