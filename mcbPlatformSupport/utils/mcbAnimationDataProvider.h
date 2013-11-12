//
//  mcbAnimationDataProvider.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 10/20/13.
//
//

#ifndef __SoundSurfer__mcbAnimationDataProvider__
#define __SoundSurfer__mcbAnimationDataProvider__

#include "mcbPath.h"
#include "mcbAudioPlayer.h"
namespace mcb{ namespace PlatformSupport{
    class AnimationDataProvider;
    typedef std::shared_ptr<AnimationDataProvider> pAnimationDataProvider;
    class AnimationDataProvider : Path{
        bool _repeatForever=false;
        bool _restoresOriginalFrame=true;
        int _repeatCount=0;
        float _fps=12.f;
        std::string _soundPath;
        std::vector<std::string> _frames;
        
        pAudioPlayer _audioPlayer=nullptr;
        
        void init(cocos2d::CCDictionary *data);
        cocos2d::CCArray * _textures=nullptr;//retained
    public:
        static pAnimationDataProvider create(const std::string & localPath, cocos2d::CCDictionary * data);
        void precacheTextures();
        cocos2d::CCAnimation * createAnimation();
        cocos2d::CCActionInterval * createAnimateAction(bool stopsSoundOnFinish=false);
        
        
        const std::vector<std::string> & frames() const{return _frames;}
        float fps() const{return _fps;}
        int repeatCount() const{return _repeatCount;}
        bool repeatForever() const{return _repeatForever;}
        bool restoresOriginalFrame() const{return _restoresOriginalFrame;}
        std::string soundPath() const{return _soundPath;}
        pAudioPlayer audioPlayer() const{return _audioPlayer;}
        cocos2d::CCArray * textures() const{return _textures;}
        
        void setRepeatForever(const bool repeatForever);
        void setRestoresOriginalFrame(const bool restoresOriginalFrame);
        void setFps(const float fps);
        void setRepeatCount(const int repeatCount);
        void setFrames(const std::vector<std::string> & frames);
        void setSoundPath(const std::string & soundPath);
        void setAudioPlayer(pAudioPlayer audioPlayer);

        AnimationDataProvider(const std::string & localPath);
        virtual ~AnimationDataProvider();
        
    };
}}

#endif /* defined(__SoundSurfer__mcbAnimationDataProvider__) */
