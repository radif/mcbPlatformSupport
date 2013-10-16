//
//  mcbAccelerometerListener.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 10/16/13.
//
//

#include "mcbAccelerometer.h"
#include "mcbPlatformSupportFunctions.h"

using namespace cocos2d;
namespace mcb{namespace PlatformSupport{
    static Accelerometer * _sharedInstance(nullptr);
    Accelerometer * Accelerometer::sharedInstance(){
        static std::once_flag onceFlag;
        std::call_once(onceFlag,[](){
            _sharedInstance=new Accelerometer;
        });
        return _sharedInstance;
    }
    void Accelerometer::didAccelerate(CCAcceleration* pAccelerationValue){
        for (const auto & listener : _listeners)
            listener.second(pAccelerationValue);
    }
    void Accelerometer::addListener(AccelerometerListener * listener, const listener_t & listenerBlock){
        _listeners[listener]=listenerBlock;
    }
    void Accelerometer::removeListener(AccelerometerListener * listener){
        auto it(_listeners.find(listener));
        if (it!=_listeners.end())
            _listeners.erase(it);
    }
    Accelerometer::Accelerometer(){
        CCDirector::sharedDirector()->getAccelerometer()->setDelegate(this);
    }
    Accelerometer::~Accelerometer(){
        CCDirector::sharedDirector()->getAccelerometer()->setDelegate(nullptr);
    }

}}
