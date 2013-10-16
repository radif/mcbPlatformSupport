//
//  mcbAccelerometer.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 10/16/13.
//
//

#ifndef __SoundSurfer__mcbAccelerometer__
#define __SoundSurfer__mcbAccelerometer__

#include "cocos2d.h"
namespace mcb{namespace PlatformSupport{
    class AccelerometerListener{};
    
    class Accelerometer : public cocos2d::CCAccelerometerDelegate{
    public:
        typedef std::function<void(cocos2d::CCAcceleration* pAccelerationValue)> listener_t;
        static Accelerometer * sharedInstance();
        void addListener(AccelerometerListener * listener, const listener_t & listenerBlock);
        void removeListener(AccelerometerListener * listener);
    private:
        std::map<AccelerometerListener *, listener_t> _listeners;
    protected:
        virtual void didAccelerate(cocos2d::CCAcceleration* pAccelerationValue) override;
        Accelerometer();
        virtual ~Accelerometer();
    };
}}
#endif /* defined(__SoundSurfer__mcbAccelerometer__) */
