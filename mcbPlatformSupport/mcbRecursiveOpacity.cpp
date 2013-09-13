//
//  mcbRecursiveOpacity.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/11/13.
//
//

#include "mcbRecursiveOpacity.h"
#include "mcbPlatformSupport.h"
#include "mcbCallLambda.h"

using namespace cocos2d;
namespace mcb{namespace PlatformSupport{
    void setRecursiveOpacity(cocos2d::CCNode * node, const GLubyte & opacity, const bool & absolute){
        //children
        CCArray * children(node->getChildren());
        if (children && children->count()) {
            mcbForEachBegin(cocos2d::CCNode *, n, children)
            setRecursiveOpacity(n, opacity, absolute);
            mcbForEachEnd
        }
        auto sprite(dynamic_cast<cocos2d::CCSprite *>(node));
        if (sprite) {
            //opacity
            if (absolute)
                sprite->setOpacity(opacity);
            else{
                GLubyte o(sprite->getOpacity());
                const float multiplier(o/255.f);
                sprite->setOpacity(opacity*multiplier);
            }
        }
        
    }
    
    
    RecursiveOpacity::RecursiveOpacity(cocos2d::CCNode * thisNode):_thisNode(thisNode){
        assert(_thisNode);
        assert(dynamic_cast<CCNode *>(_thisNode));
    }
    RecursiveOpacity::~RecursiveOpacity(){}

    
    void RecursiveOpacity::setOpacityRecursively(const GLubyte opacity, const bool absolute){
            setRecursiveOpacity(_thisNode, opacity, absolute);
    }
    
    void RecursiveOpacity::runFadeAction(const float duration, const GLubyte fromOpacity, const GLubyte toOpacity, const std::function<void()> & completion){
        int delta(toOpacity-fromOpacity);
        _thisNode->runAction(PlatformSupport::create_scheduleLambda(0.f, [=](float dt, float * pActionTime, bool &stop){
            (*pActionTime)+=dt;
            if (*pActionTime>=duration) {
                setOpacityRecursively(toOpacity);
                if (completion)
                    completion();
                stop=true;
            }
            float progress(*pActionTime/duration);
            //float curvedProgress(1.f-cos(progress*M_PI_2));
            setOpacityRecursively(fromOpacity+delta*progress);
        }));
    
    
    }
    
    void RecursiveOpacity::runVeilInAction(const float duration, const std::function<void()> & completion, const cocos2d::ccColor4B & color, const int zOrder){
        addVeil();
        _veil->setOpacity(0);
        _veil->runAction(CCSequence::createWithTwoActions(CCFadeIn::create(duration), PlatformSupport::CallLambda::create([=](){
            if (completion)
                completion();
        })));
        
    }
    void RecursiveOpacity::runVeilOutAction(const float duration, const std::function<void()> & completion){
        if (!_veil) {
            if (completion)
                completion();
            return;
        }
        _veil->stopAllActions();
        _veil->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(duration), PlatformSupport::CallLambda::create([=](){
            removeVeil();
            if (completion)
                completion();
        })));
    }
    void RecursiveOpacity::addVeil(const cocos2d::ccColor4B & color, const int zOrder){
        if (_veil){
            _veil->stopAllActions();
            _veil->setColor({color.r, color.g, color.b});
            _veil->setOpacity(255);
            _thisNode->reorderChild(_veil, zOrder);
        }else{
            CCSize winSize(CCDirector::sharedDirector()->getWinSize());
            _veil=CCLayerColor::create(color, winSize.width, winSize.height);
            _thisNode->addChild(_veil,zOrder);
        }
    }
    void RecursiveOpacity::removeVeil(){
        if (_veil) {
            _thisNode->removeChild(_veil,true);
            _veil=nullptr;
        }
    }


}}
