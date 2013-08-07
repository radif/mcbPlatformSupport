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
    
    void AudioPlayer::playOneShot(const std::string & audioFile){
        AudioPlayer *p(new AudioPlayer(audioFile));
        p->m_playerCompletion=[=](AudioPlayer *m_p){delete m_p;};
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
}}