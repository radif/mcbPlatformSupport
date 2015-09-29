//
//  mcbOpenALPlayer.h
//  EndlessReader
//
//  Created by Radif Sharafullin on 8/12/13.
//
//

#ifndef __EndlessReader__mcbOpenALPlayer__
#define __EndlessReader__mcbOpenALPlayer__

#include <string>
#include <functional>
#include <vector>

namespace mcb{namespace PlatformSupport{
    class OpenALPlayer{
        
        void __platformSpecificInit(const std::string & audioFile, bool loops);
        void __platformSpecificCleanup();
        
        void * __nativeHolder=nullptr;
        
        const bool _useReverb=false;
        const std::string _audioPath;
        const bool _loops=false;
        
    public:
        
        struct Direction{float x; float y; float z;};
        
        //lifecycle
        OpenALPlayer(const std::string & audioFile, bool useReverb=false, bool loops=0);
        virtual ~OpenALPlayer();
        
        /* transport control */
        /* methods that return bool return true on success and false on failure. */

        void play();            /* sound is played asynchronously. */
//        void playAtTime(float time);    /* play a sound some time in the future. time should be greater than deviceCurrentTime. */
        void pause(){setRate(0);}
        void stop();
        
        
        /* properties */
        bool getIsPlaying() const;
//        int getNumberOfChannels() const;
        float getDuration() const;
        
        //reverb
        void setGlobalReverbLevel(const float reverbLevel);
        void setRoomType(const int roomType);
        void setReverbObstruction(const float reverbObstruction);
        void setReverbOcclusion(const float reverbOcclusion);
        void setReverbSendLevel(const float reverbSendLevel);
        
        float getReverbObstruction() const;
        float getReverbOcclusion() const;
        float getReverbSendLevel() const;
        
        void setPan(float pan); /* set panning. -1.0 is left, 0.0 is center, 1.0 is right. */
        float getPan() const;
        
        void setRate(float rate){setPitch(rate);}
        float getRate() const{return getPitch();}
        
        void setVolume(const float & volume); /* The volume for the sound. The nominal range is from 0.0 to 1.0. */
        float getVolume() const;

        void setPitch(float pitch);
        float getPitch() const;

        void setCurrentTime(float currentTime);
        float getCurrentTime() const;
        
        void setDirection(const Direction & d);
        Direction getDirection() const;
        
        void setMeteringEnabled(bool enabled){}
        bool isMeteringEnabled(){return false;}
        
        void updateMeters(){}
        
        float getPeakPowerForChannel(int channelNumber){return 0.f;}
        float getAveragePowerForChannel(int channelNumber){return 0.f;}

        
        static void playEffect(const std::string & audioFile);
        static void preloadEffect(const std::string & audioFile);
        
        const std::string & audioPath() const{return _audioPath;}
    };
    typedef std::shared_ptr<OpenALPlayer> pOpenALPlayer;
    typedef std::weak_ptr<OpenALPlayer> pWeakOpenALPlayer;
}}
#endif /* defined(__EndlessReader__mcbOpenALPlayer__) */
