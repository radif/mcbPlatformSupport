//
//  mcbAccelerometerParallaxNode.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 10/16/13.
//
//

#include "mcbAccelerometerParallaxNode.h"

using namespace cocos2d;
namespace mcb{namespace PlatformSupport{
#pragma mark view cycle
    void AccelerometerParallaxNode::setPosition(const cocos2d::CCPoint & pos){
        _realPosition=pos;
        super::setPosition(ccpAdd(_realPosition, _offset));
    }
    const cocos2d::CCPoint & AccelerometerParallaxNode::getPosition(){
        return _realPosition;
    }
    void AccelerometerParallaxNode::onEnter(){
        super::onEnter();
        Accelerometer::sharedInstance()->addListener(this, [=](cocos2d::CCAcceleration* pAccelerationValue){
            _prevAccel.x = (pAccelerationValue->x*_filteringFactor)+(_prevAccel.x*(1.f-_filteringFactor));
            _prevAccel.y = (pAccelerationValue->y*_filteringFactor)+(_prevAccel.y*(1.f-_filteringFactor));
            
            _offset.x=(_prevAccel.x+_accelerationOffset.x)*_multiplier.x;
            _offset.y=(_prevAccel.y+_accelerationOffset.y)*_multiplier.y;
            super::setPosition(ccpAdd(_realPosition, _offset));
        });
    }
    void AccelerometerParallaxNode::onExit(){
        super::onExit();
        Accelerometer::sharedInstance()->removeListener(this);
    }

}}
