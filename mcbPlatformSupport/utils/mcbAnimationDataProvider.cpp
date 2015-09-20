//
//  mcbAnimationDataProvider.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 10/20/13.
//
//

#include "mcbAnimationDataProvider.h"
#include "mcbPlatformSupportFunctions.h"
#include "mcbAudioPlayer.h"
#include "mcbCallLambda.h"

using namespace cocos2d;
namespace mcb{ namespace PlatformSupport{
    AnimationDataProvider::AnimationDataProvider(const std::string & localPath) : Path(localPath){}
    AnimationDataProvider::~AnimationDataProvider(){
        CC_SAFE_RELEASE(_textures);
        CC_SAFE_RELEASE(_animation);
    }
    pAnimationDataProvider AnimationDataProvider::create(const std::string & localPath, cocos2d::CCDictionary * data){
        if (!data)
            return nullptr;
        
        auto retVal(std::make_shared<AnimationDataProvider>(localPath));
        retVal->init(data);
        return retVal;
    }
    void AnimationDataProvider::init(cocos2d::CCDictionary *data){
        _fps=Functions::floatForObjectKey(data, "fps", _fps);
        _soundPath=mcbPath(Functions::stringForObjectKey(data, "sound", _soundPath));
        _repeatCount=Functions::floatForObjectKey(data, "repeatCount", _repeatCount);
        _repeatForever=Functions::boolForObjectKey(data, "repeatForever", _repeatForever);
        _restoresOriginalFrame=Functions::boolForObjectKey(data, "restoresOriginalFrame", _restoresOriginalFrame);

        
        CCArray * animationA((CCArray *)data->objectForKey("frames"));
        if (animationA) {
            _frames.reserve(animationA->count());
            mcbForEachBegin(CCString *, path, animationA);
            path=mcbPath(path);
            _frames.push_back(path->m_sString);
            mcbForEachEnd
        }else{
            CCString * framesConvention((CCString *)data->objectForKey("framesConvention"));
            if (framesConvention) {
                int maxFrameNumber(mcb::PlatformSupport::Functions::floatForObjectKey(data, "maxFrameNumber"));
                
                _frames.reserve(maxFrameNumber);
                const std::string basePath(mcbPath(framesConvention->m_sString));
                for (int frameNumber(0); frameNumber<=maxFrameNumber; ++frameNumber) {
                    std::string path(basePath);
                    char buffer [path.size()+10];
                    sprintf(buffer, path.c_str(), frameNumber);
                    path=buffer;
                    _frames.push_back(path);
                }
                
            }
            
        }
    }
    void AnimationDataProvider::precacheTextures(){
        if (!_textures) {
            if (!_frames.empty()) {
                _textures=CCArray::create();
                _textures->retain();
                for (const std::string & path : _frames)
                    _textures->addObject(CCTextureCache::sharedTextureCache()->addImage(path.c_str()));
            }
        }
    }
    cocos2d::CCAnimation * AnimationDataProvider::createAnimation(){
        if (_animation)
            return _animation;
        
        precacheTextures();
        
        CCAnimation * retVal(CCAnimation::create());
        retVal->setDelayPerUnit(1.f/_fps);
        
        auto addTextureL([=](CCTexture2D *tex){
            CCRect rect;
            rect.origin=CCPointZero;
            rect.size=tex->getContentSize();
            retVal->addSpriteFrameWithTexture(tex, rect);
        });
        
        if (_textures) {
            unsigned count(_textures->count());
            for (int i=0; i<count; ++i)
                addTextureL((CCTexture2D *)_textures->objectAtIndex(i));
            if (_restoresOriginalFrame)
                addTextureL((CCTexture2D *)_textures->objectAtIndex(0));
        }
        _animation=retVal;
        _animation->retain();
        return retVal;
    }
    cocos2d::CCActionInterval * AnimationDataProvider::createAnimateAction(bool stopsSoundOnFinish){
        _audioPlayer=nullptr;
        CCActionInterval *retVal(CCAnimate::create(createAnimation()));
        
        
        //sound
        if (!_soundPath.empty()) {
            std::string soundCopy(_soundPath);
            _audioPlayer=std::make_shared<PlatformSupport::AudioPlayer>(soundCopy);
            pAudioPlayer playerCopy(_audioPlayer);
            retVal=CCSequence::createWithTwoActions(CallLambda::create([=](){
                playerCopy->play();
            }), retVal);
            if (stopsSoundOnFinish)
                retVal=CCSequence::createWithTwoActions(retVal, CallLambda::create([=](){
                    if (playerCopy) {
                        playerCopy->stop();
                        //_audioPlayer=nullptr;
                    }
                }));
        }
        
        //repeat
        if (_repeatForever)
            retVal=CCRepeatForever::create(retVal);
        else if(_repeatCount)
            retVal=CCRepeat::create(retVal, _repeatCount);
        
        return retVal;
    }
    void AnimationDataProvider::setRestoresOriginalFrame(const bool restoresOriginalFrame){_restoresOriginalFrame=restoresOriginalFrame;}
    void AnimationDataProvider::setRepeatForever(const bool repeatForever){_repeatForever=repeatForever;}
    void AnimationDataProvider::setFps(const float fps){_fps=fps;}
    void AnimationDataProvider::setRepeatCount(const int repeatCount){_repeatCount=repeatCount;}
    void AnimationDataProvider::setFrames(const std::vector<std::string> & frames){_frames=frames;}
    void AnimationDataProvider::setSoundPath(const std::string & soundPath){_soundPath=soundPath;}
    void AnimationDataProvider::setAudioPlayer(pAudioPlayer audioPlayer){_audioPlayer=audioPlayer;}
}}