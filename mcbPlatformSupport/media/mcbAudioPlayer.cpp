//
//  AudioPlayer.cpp
//  mcb
//
//  Created by Radif Sharafullin on 7/01/13.
//
//

#include "mcbAudioPlayer.h"

//#define PLAYER_DEBUG_INSTANCE_COUNTING 1

#if defined(PLAYER_DEBUG_INSTANCE_COUNTING) && PLAYER_DEBUG_INSTANCE_COUNTING==1
#include <iostream>
#endif

namespace mcb{namespace PlatformSupport{
#if defined(PLAYER_DEBUG_INSTANCE_COUNTING) && PLAYER_DEBUG_INSTANCE_COUNTING==1
    static int instanceCounter=0;
#endif
    AudioPlayer::AudioPlayer(const std::string & audioFile, const int & numberOfLoops)
    : AudioPlayer("", audioFile, 0.f,1.f,numberOfLoops)
    {
        
    }
    AudioPlayer::AudioPlayer(const std::string & soundId, const std::string & audioFile, const float & time, const float & volume, const int & numberOfLoops)
    : _soundId(soundId)
    , _audioFile(audioFile)
    {
#if defined(PLAYER_DEBUG_INSTANCE_COUNTING) && PLAYER_DEBUG_INSTANCE_COUNTING==1
        instanceCounter++;
        std::cout << "player instance counter " << instanceCounter<<std::endl;
#endif
        __platformSpecificAllocPlayer();
        setCurrentTime(time);
        setVolume(volume);
        setNumberOfLoops(numberOfLoops);
    }
    AudioPlayer::~AudioPlayer(){
#if defined(PLAYER_DEBUG_INSTANCE_COUNTING) && PLAYER_DEBUG_INSTANCE_COUNTING==1
        instanceCounter--;
        std::cout << "player instance counter " << instanceCounter<<std::endl;
#endif
        __platformSpecificDeletePlayer();
    }
    
    void AudioPlayer::playOneShot(const std::string & audioFile, const std::function<void()> & completion, const std::function<void(AudioPlayer * player)> playerSetupHandle){
        AudioPlayer *p(new AudioPlayer(audioFile));
        if (playerSetupHandle)
            playerSetupHandle(p);
        p->m_playerCompletion=[=](AudioPlayer *m_p){if(completion) completion(); delete m_p;};
        p->play();
    }
    
    void AudioPlayer::__playerdidFinishPlaying(bool successfully){
        if (m_playerFinishedHandle)
            m_playerFinishedHandle(this, successfully);
    }
    void AudioPlayer::setSuspended(bool suspended){
        if (_isSuspended==suspended)
            return;

        if (suspended) {
            _wasPlayingBeforeSuspension=getIsPlaying();
            pause();
            _isSuspended=suspended;
        }else{
            if (_wasPlayingBeforeSuspension) {
                _isSuspended=suspended;
                play();
            }
            _wasPlayingBeforeSuspension=false;
        }
        
    }
    
#pragma mark -
#pragma mark EffectPlayer
    
    EffectPlayer::EffectPlayer(const std::string filePath, unsigned capacity): m_filePath(filePath){
        for (int i(0); i<capacity; ++i)
            _players.push_back(createPlayer());
        
    }
    EffectPlayer::~EffectPlayer(){
        for (auto player : _players) {
            player->m_playerCompletion=nullptr;
            player->stop();
        }
    }
    void EffectPlayer::play(){
        pAudioPlayer player(nullptr);
        if (_players.empty())
            player=createPlayer();
        else{
            player=_players.back();
            _players.pop_back();
        }
        
        player->play();
        player->m_playerCompletion=[=](AudioPlayer *p){
            _players.push_back(player);
            player->m_playerFinishedHandle=nullptr;
        };
    }
    
    pAudioPlayer EffectPlayer::createPlayer(){
        pAudioPlayer player(std::make_shared<AudioPlayer>(m_filePath));
        player->prepareToPlay();
        return player;
    }
    
}}