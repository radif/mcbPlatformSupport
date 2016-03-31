//
//  AudioPlayer.h
//  mcb
//
//  Created by Radif Sharafullin on 7/01/13.
//
//

#ifndef __mcb__AudioPlayer__
#define __mcb__AudioPlayer__

#include <string>
#include <functional>
#include <vector>

namespace mcb{namespace PlatformSupport{
    
    class AudioPlayer{
        const std::string _soundId;
        const std::string _audioFile;
        
        bool _wasPlayingBeforeSuspension=false;
        bool _isSuspended=false;
        void * __platformSpecificPlayerPointer=nullptr;
        void __platformSpecificAllocPlayer();
        void __platformSpecificDeletePlayer();
        void __playerdidFinishPlaying(bool successfully);
        
        bool _playerCompletionIsCalled=false;
        void _callPlayerCompletion();
    public:
        //lifecycle
        AudioPlayer(const std::string & soundId, const std::string & audioFile, const float & time, const float & volume, const int & numberOfLoops);
        AudioPlayer(const std::string & audioFile, const int & numberOfLoops=0);
        virtual ~AudioPlayer();
        
        
        static void playOneShot(const std::string & audioFile, const std::function<void()> & completion=nullptr, const std::function<void(AudioPlayer * player)> playerSetupHandle=nullptr);
        
        /* transport control */
        /* methods that return bool return true on success and false on failure. */
        bool prepareToPlay();   /* get ready to play the sound. happens automatically on play. */
        void play();            /* sound is played asynchronously. */
        void playAtTime(float time);    /* play a sound some time in the future. time should be greater than deviceCurrentTime. */
        void pause();           /* pauses playback, but remains ready to play. */
        void stop();            /* stops playback. no longer ready to play. */
        
        
        /* properties */
        std::string getSoundID() const{return _soundId;}
        std::string getAudioPath() const{return _audioFile;}
        bool getIsPlaying() const; /* is it playing or not? */
        int getNumberOfChannels() const;
        float getDuration() const; /* the duration of the sound. */
        
        void setPan(float pan); /* set panning. -1.0 is left, 0.0 is center, 1.0 is right. */
        float getPan() const;
        
        
        void setVolume(const float & volume); /* The volume for the sound. The nominal range is from 0.0 to 1.0. */
        float getVolume() const;
        
        void setRate(float rate); /* The playback rate for the sound. 1.0 is normal, 0.5 is half speed, 2.0 is double speed. */
        float getRate() const;
        
        /*  If the sound is playing, currentTime is the offset into the sound of the current playback position.
         If the sound is not playing, currentTime is the offset into the sound where playing would start. */
        void setCurrentTime(float currentTime);
        float getCurrentTime() const;
        
        //TODO: do we need that?
        /* returns the current time associated with the output device */
        float getDeviceCurrentTime() const;
        
        
        /* "numberOfLoops" is the number of times that the sound will return to the beginning upon reaching the end.
         A value of zero means to play the sound just once.
         A value of one will result in playing the sound twice, and so on..
         Any negative number will loop indefinitely until stopped.
         */
        void setNumberOfLoops(int numberOfLoops);
        int getNumberOfLoops() const;
        
        void setMeteringEnabled(bool enabled);/* turns level metering on or off. default is off. */
        bool isMeteringEnabled();
        
        void updateMeters();/* call to refresh meter values */
        
        float getPeakPowerForChannel(int channelNumber); /* returns peak power in decibels for a given channel */
        float getAveragePowerForChannel(int channelNumber); /* returns average power in decibels for a given channel */

        
        /* audioPlayerDidFinishPlaying:successfully: is called when a sound has finished playing. This block is NOT called if the player is stopped due to an interruption. */
        std::function<void(AudioPlayer *, bool)> m_playerFinishedHandle=nullptr;
        
        //this is guaranted to be called
        std::function<void(AudioPlayer *)> m_playerCompletion=nullptr;

        
        //backgrounding

        void setSuspended(bool suspended);
        
        int m_soundId=0;
        
    };
    typedef std::shared_ptr<AudioPlayer> pAudioPlayer;
    typedef std::weak_ptr<AudioPlayer> pWeakAudioPlayer;

    
    class EffectPlayer{
        std::vector<pAudioPlayer> _players;
        pAudioPlayer createPlayer();
    public:
        const std::string m_filePath;
        EffectPlayer(const std::string filePath, unsigned capacity=3);
        virtual ~EffectPlayer();
        void play();
    };
    typedef std::shared_ptr<EffectPlayer> pEffectPlayer;
    
    class VarispeedPlayer{
        void * _nativeHandler=nullptr;
    public:
        VarispeedPlayer(const std::string filePath, bool loop=false);
        virtual ~VarispeedPlayer();
        
        std::function<void(VarispeedPlayer * player)> onPlayToEnd=nullptr;
        
        void prepare();
        void play();
        void pause();
        void stop();
        
        bool isPlaying() const;
        
        void setVolume(const float volume);
        float volume() const;
        
        void setRate(const float rate);
        float rate() const;
        
        void seekToTime(const float time);
        
        std::string audioPath() const;
        bool loop() const;
        
        //playback props
        float currentTime() const;
        float duration() const;
    };
    typedef std::shared_ptr<VarispeedPlayer> pVarispeedPlayer;
    
    
}}
#endif /* defined(__mcb__AudioPlayer__) */
