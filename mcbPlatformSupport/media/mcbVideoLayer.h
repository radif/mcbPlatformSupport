//
//  mcbVideoLayer.h
//  ColorDots
//
//  Created by Radif Sharafullin on 11/4/13.
//
//

#ifndef __ColorDots__mcbVideoLayer__
#define __ColorDots__mcbVideoLayer__

#include "cocos2d.h"
namespace mcb{namespace PlatformSupport{
    class VideoLayer;
    typedef std::function<void(VideoLayer *)> VideoLayerCallback_t;
    class VideoLayer : public cocos2d::CCNode{
        typedef cocos2d::CCNode super;
        std::string _videoPath;
        cocos2d::CCRect _videoFrame;
        void * _nativeHandler=nullptr;
    public:
        
        static VideoLayer * create();
        static VideoLayer * create(const std::string & videoPath);
        static VideoLayer * create(const std::string & videoPath, const cocos2d::CCRect & videoFrame);
        static VideoLayer * create(const cocos2d::CCRect & videoFrame);
        
        //unimplemented
        /*
        VideoLayerCallback_t onLoaded=nullptr;
        VideoLayerCallback_t onPlaybackStarted=nullptr;
        VideoLayerCallback_t onPlaybackWillFinish=nullptr;
        */
        
        VideoLayerCallback_t onPlaybackFinished=nullptr;
        
        void start();
        void pause();
        void resume();
        void stop();
        void seek(float time);
        
        float duration() const;
        bool isPlaying() const;
        float currentTime() const;
        void sizeToFill(const cocos2d::CCSize & originalSize);
        const std::string & videoPath() const;
        void setVideoPath(const std::string & videoPath);
        void setVideoFrame(const cocos2d::CCRect & videoFrame);
        const cocos2d::CCRect & videoFrame() const;
    protected:
        virtual void onEnter() override;
        virtual void onExit() override;
#if COCOS2D_VERSION <= 0x00020003
        virtual bool init();
#else
        virtual bool init() override;
#endif
        VideoLayer();
        virtual ~VideoLayer();
    };
}}
#endif /* defined(__ColorDots__mcbVideoLayer__) */
