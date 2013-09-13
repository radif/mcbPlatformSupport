//
//  mcbAnimationFunctions.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/12/13.
//
//

#ifndef __SoundSurfer__mcbAnimationFunctions__
#define __SoundSurfer__mcbAnimationFunctions__

#include "cocos2d.h"
namespace mcb{ namespace PlatformSupport{
    class mcbEaseBackInOut : public cocos2d::CCEaseBackInOut{
        const float _overshot;
    public:
        virtual void update(float time);
        mcbEaseBackInOut(float overshot);
        virtual ~mcbEaseBackInOut();
    public:
        /** creates the action */
        static mcbEaseBackInOut* create(CCActionInterval* pAction, float overshot=1.f);
    };
}}

#endif /* defined(__SoundSurfer__mcbAnimationFunctions__) */
