//
//  mcbAccelerometerParallaxNode.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 10/16/13.
//
//

#ifndef __SoundSurfer__mcbAccelerometerParallaxNode__
#define __SoundSurfer__mcbAccelerometerParallaxNode__

#include "cocos2d.h"
#include "mcbAccelerometer.h"

namespace mcb{namespace PlatformSupport{
    class AccelerometerParallaxNode : public cocos2d::CCNode, public AccelerometerListener{
        typedef cocos2d::CCNode super;
        cocos2d::CCPoint _multiplier={0.f,0.f};
        cocos2d::CCPoint _realPosition;
        cocos2d::CCPoint _offset={0.f,0.f};
        std::string _accelerometerListenerKey;
    public:
        CREATE_FUNC(AccelerometerParallaxNode);
        void setMultiplier(const cocos2d::CCPoint & multiplier);
        const cocos2d::CCPoint & multiplier();
        virtual void setPosition(const cocos2d::CCPoint & pos);
        virtual const cocos2d::CCPoint & getPosition() override;
    protected:
        virtual void onEnter() override;
        virtual void onExit() override;
    };
}}
#endif /* defined(__SoundSurfer__mcbAccelerometerParallaxNode__) */
