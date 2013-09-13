//
//  mcbRecursiveOpacity.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/11/13.
//
//

#ifndef __SoundSurfer__mcbRecursiveOpacity__
#define __SoundSurfer__mcbRecursiveOpacity__

#include "cocos2d.h"
#include <functional>

namespace mcb{namespace PlatformSupport{
    struct RecursiveOpacity{
        void setOpacityRecursively(const GLubyte opacity, const bool absolute=false);
        void runFadeAction(const float duration, const GLubyte fromOpacity, const GLubyte toOpacity, const std::function<void()> & completion=nullptr);
        
        void runVeilInAction(const float duration, const std::function<void()> & completion=nullptr, const cocos2d::ccColor4B & color={0,0,0,255}, const int zOrder=99999);
        void runVeilOutAction(const float duration, const std::function<void()> & completion=nullptr);
        void addVeil(const cocos2d::ccColor4B & color={0,0,0,255}, const int zOrder=99999);
        void removeVeil();
        cocos2d::CCLayerColor * getVeil(){return _veil;}
        
        RecursiveOpacity(cocos2d::CCNode * thisNode);
        virtual ~RecursiveOpacity();
        
    protected:
        cocos2d::CCLayerColor * _veil=nullptr;
    private:
        cocos2d::CCNode * const _thisNode;
    };
    
    void setRecursiveOpacity(cocos2d::CCNode * node, const GLubyte & opacity, const bool & absolute=true);

}}


#endif /* defined(__SoundSurfer__mcbRecursiveOpacity__) */
