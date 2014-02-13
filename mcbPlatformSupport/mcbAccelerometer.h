//
//  mcbAccelerometer.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 10/16/13.
//
//

#ifndef __SoundSurfer__mcbAccelerometer__
#define __SoundSurfer__mcbAccelerometer__

#include "mcbFactory.h"

namespace mcb{namespace PlatformSupport{
    class AccelerometerListener{};
    
    class Accelerometer : public PlatformSupport::SingletonFactory<Accelerometer> , public cocos2d::CCAccelerometerDelegate{
        friend PlatformSupport::SingletonFactory<Accelerometer>;
        class{
            bool _initialized=false;
            cocos2d::CCAcceleration value;
        public:
            void set(cocos2d::CCAcceleration * pAcceleration){value=*pAcceleration;_initialized=true;}
            cocos2d::CCAcceleration * get(){return &value;}
            bool isInitialized() const{return _initialized;}
        }_lastAcceleration;
    public:
        typedef std::function<void(cocos2d::CCAcceleration* pAccelerationValue)> listener_t;
        void addListener(AccelerometerListener * listener, const listener_t & listenerBlock);
        void addListener(AccelerometerListener * listener, listener_t && listenerBlock) noexcept;
        void removeListener(AccelerometerListener * listener);
    private:
        std::map<AccelerometerListener *, listener_t> _listeners;
    protected:
        virtual void init() override{}
        virtual void didAccelerate(cocos2d::CCAcceleration* pAccelerationValue) override;
        Accelerometer();
        virtual ~Accelerometer();
    };
}}
#endif /* defined(__SoundSurfer__mcbAccelerometer__) */
