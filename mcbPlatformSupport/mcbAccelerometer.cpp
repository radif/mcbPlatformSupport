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
    void Accelerometer::didAccelerate(CCAcceleration* pAccelerationValue){
        for (const auto & listener : _listeners)
            listener.second(pAccelerationValue);
        _lastAcceleration.set(pAccelerationValue);
    }
    void Accelerometer::addListener(AccelerometerListener * listener, const listener_t & listenerBlock){
        if (_lastAcceleration.isInitialized())
            listenerBlock(_lastAcceleration.get());
        _listeners[listener]=listenerBlock;
    }
    void Accelerometer::addListener(AccelerometerListener * listener, listener_t && listenerBlock) noexcept{
        if (_lastAcceleration.isInitialized())
            listenerBlock(_lastAcceleration.get());//noexcept needed for move, but can we guarantee noexcept here?
        _listeners[listener]=std::forward<listener_t>(listenerBlock);
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
