//
//  mcbAnimationFunctions.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/12/13.
//
//

#include "mcbAnimationFunctions.h"
using namespace cocos2d;
namespace mcb{ namespace PlatformSupport{
    mcbEaseBackInOut::mcbEaseBackInOut(float overshot):_overshot(overshot){}
    mcbEaseBackInOut::~mcbEaseBackInOut(){}

    void mcbEaseBackInOut::update(float time){
        float overshoot(1.70158f * 1.525f *_overshot);
        time = time * 2;
        if (time < 1){
            m_pInner->update((time * time * ((overshoot + 1) * time - overshoot)) / 2);
        }
        else{
            time = time - 2;
            m_pInner->update((time * time * ((overshoot + 1) * time + overshoot)) / 2 + 1);
        }
    }
    
    /** creates the action */
    mcbEaseBackInOut* mcbEaseBackInOut::create(CCActionInterval* pAction, float overshot){
        mcbEaseBackInOut *pRet(new mcbEaseBackInOut(overshot));
        if (pRet){
            if (pRet->initWithAction(pAction))
                pRet->autorelease();
            else
                CC_SAFE_RELEASE_NULL(pRet);
        }
        return pRet;
    }
}}
