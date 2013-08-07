//
//  mcbAnimationSprite.cpp
//  PlatformSupport
//
//  Created by Radif Sharafullin on 7/24/13.
//
//

#include "mcbAnimationSprite.h"
namespace mcb{ namespace PlatformSupport{
    AnimationSprite::AnimationData::~AnimationData(){
        for (auto tex: _texures)
            CC_SAFE_RELEASE(tex);
    }
    
    AnimationSprite* AnimationSprite::create(const char *pszFileName){
        AnimationSprite *pobSprite = new AnimationSprite();
        if (pobSprite && pobSprite->initWithFile(pszFileName))
        {
            pobSprite->autorelease();
            return pobSprite;
        }
        CC_SAFE_DELETE(pobSprite);
        return nullptr;
    }
    
    void AnimationSprite::AnimationData::addTexture(cocos2d::CCTexture2D * tex){
        assert(tex);
        tex->retain();
        _texures.push_back(tex);
    }
    
    void AnimationSprite::setAnimationDataForKey(pAnimationData animData, const std::string & key){
        _animations[key]=animData;
    }
    void AnimationSprite::setAllAnimationFrameDelay(float frameDelay){
        for (const auto & p : _animations)
            p.second->_frameDelay=frameDelay;
    }
    void AnimationSprite::setAnimationFrameDelayForAnimationKey(float frameDelay, const std::string & key){
        auto it (_animations.find(key));
        if (it!=_animations.end())
            (*it).second->_frameDelay=frameDelay;
    }
    void AnimationSprite::playAnimationForKey(const std::string & key, const std::function<void()> & cycleCompletion, bool reset){
        auto it (_animations.find(key));
        if (it!=_animations.end()){
            _currentAnimation=(*it).second;
            if (reset)
                _currentAnimation->reset();
            _currentAnimation->_cycleCompletion=cycleCompletion;
            _isAnimating=true;
        }
    }
#pragma mark update
    void AnimationSprite::onEnter(){
        super::onEnter();
        scheduleUpdate();
    }
    void AnimationSprite::onExit(){
        super::onExit();
        unscheduleUpdate();
    }
    void AnimationSprite::update(float dt){
        if (_currentAnimation && _isAnimating && m_bVisible) {
            setTexture(_currentAnimation->textureForCurrentFrame());
            _currentAnimation->addDeltaTime(dt);
        }
    }
    
}}