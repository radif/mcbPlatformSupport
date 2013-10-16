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
            _offset.x=pAccelerationValue->x*_multiplier.x;
            _offset.y=pAccelerationValue->y*_multiplier.y;
            super::setPosition(ccpAdd(_realPosition, _offset));
        });
    }
    void AccelerometerParallaxNode::onExit(){
        super::onExit();
        Accelerometer::sharedInstance()->removeListener(this);
    }
    
    void AccelerometerParallaxNode::setMultiplier(const cocos2d::CCPoint & multiplier){
        _multiplier=multiplier;
    }
    const cocos2d::CCPoint & AccelerometerParallaxNode::multiplier(){
        return _multiplier;
    }

}}
