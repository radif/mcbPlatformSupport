//
//  mcbTimedProperty.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 2/1/14.
//
//

#ifndef __mcb__PlatformSupport__TimedProperty__
#define __mcb__PlatformSupport__TimedProperty__

#include <iostream>
namespace mcb{ namespace PlatformSupport{
    
    //this class is using system time to measure the duration and expire. It is read only, no event supported!
    //works with time_t, not floats, subsecond granularity is impossible
    class TimedProperty{
        static inline time_t now(){return time(nullptr);}
        const time_t kNever=0;
        mutable time_t _activeUntil=kNever;
        time_t _suspendedTime=kNever;
        time_t _defaultDuration=kNever;
    public:
        TimedProperty()=default;
        TimedProperty(time_t defaultDuration): _defaultDuration(defaultDuration){}
        void activateFor(time_t duration){_activeUntil=now()+duration;}
        void activate(){activateFor(_defaultDuration);}
        void deactivate(){_activeUntil=kNever;}
        operator bool() const{return isActive();}
        bool isActive() const{
            if (isSuspended())
                return _activeUntil!=kNever;
            bool retVal(now()<=_activeUntil);
            if (!retVal)
                _activeUntil=kNever;
            return retVal;
        }
        time_t isActiveFor() const{
            if (!isActive())
                return kNever;
            return _activeUntil-now();
        }
        bool isSuspended() const{return _suspendedTime!=kNever;}
        void setSuspended(bool suspended){
            if (suspended==isSuspended())
                return;
            if (suspended) {
                isActive();//reset is needed for activeUntil to query isActive() during suspension and for resume from suspension
                _suspendedTime=now();
            }else{
                if (isActive())
                    _activeUntil=now()+(_activeUntil-_suspendedTime);
                _suspendedTime=kNever;
            }
        }
    };
    
    //this class works with float and makes possible subsecond granularity. But it requires the developer to call tick() and provide delta time. It is read only, no event supported!
    class TimedPropertyTick{
        float _defaultDuration=0.f;
        float _currentDuration=-HUGE_VALF;
        float _elapsedTime=0.f;
        bool _isSuspended=false;
    public:
        TimedPropertyTick()=default;
        TimedPropertyTick(float defaultDuration): _defaultDuration(defaultDuration){}
        void activateFor(float duration){_currentDuration=duration;_elapsedTime=0.f;}
        void activate(){activateFor(_defaultDuration);}
        void deactivate(){_currentDuration=-HUGE_VALF;}
        operator bool() const{return isActive();}
        bool isActive() const{return _elapsedTime<=_currentDuration;}
        float isActiveFor() const{
            if (!isActive())
                return 0.f;
            return _currentDuration-_elapsedTime;
        }
        void tick(float deltaTime){if(!_isSuspended) _elapsedTime+=deltaTime;}
        bool isSuspended() const{return _isSuspended;}
        void setSuspended(bool suspended){_isSuspended=suspended;}
    };

}}
#endif /* defined(__mcb__PlatformSupport__TimedProperty__) */
