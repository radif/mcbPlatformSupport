//
//  mcbCallLambda.h
//  mcb
//
//  Created by Radif Sharafullin on 6/8/13.
//
//
#ifndef __mcb__mcbCallLambda__
#define __mcb__mcbCallLambda__

#include "cocos2d.h"

namespace mcb{namespace PlatformSupport{
    class CallLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void()> _lambda=nullptr;
        virtual void update(float time) {execute();}
    public:
        static CallLambda * create(const std::function<void()> & lambda, int tag=0);
        static cocos2d::CCSequence * createWithDelay(const float delay, const std::function<void()> & lambda, int tag=0);
        virtual bool initWithLambda(const std::function<void()> & lambda);
        virtual void execute();
    protected:
        CallLambda() = default;
        virtual ~CallLambda(){}
    };
    
    template <typename T>
    class ScheduleLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void(const float deltaTime, T *userData , bool & stop)> _lambda=nullptr;
        virtual void update(float deltaTime) {execute(deltaTime);}
        bool _isDone=false;
        virtual bool isDone(){return _isDone;}
        virtual void step(float dt){update(dt);}
        T _userData;
    public:
        static ScheduleLambda * create(T && userData, std::function<void(const float deltaTime, T *userData , bool & stop)> &&lambda){
            ScheduleLambda *retVal = new ScheduleLambda();
            if (retVal){
                retVal->_userData=std::forward<T>(userData);
                retVal->_lambda=std::forward<decltype(lambda)>(lambda);
                retVal->autorelease();
                return retVal;
            }
            CC_SAFE_DELETE(retVal);
            return nullptr;
        }
        
        virtual void execute(float deltaTime){
            if (_lambda)
                _lambda(deltaTime, &_userData,_isDone);
        }
    protected:
        ScheduleLambda() = default;
        virtual ~ScheduleLambda()=default;
    };
    template <typename T>
    ScheduleLambda<T> * create_scheduleLambda(T userData, std::function<void(float deltaTime, decltype(userData) *userData , bool & stop)> && lambda){
        return ScheduleLambda<T>::create(std::forward<T>(userData), std::forward<decltype(lambda)>(lambda));
    }
    
    
    class ScheduleUpdateLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void(const float deltaTime, bool & stop)> _lambda=nullptr;
        virtual void update(float deltaTime) {execute(deltaTime);}
        bool _isDone=false;
        virtual bool isDone(){return _isDone;}
        virtual void step(float dt){update(dt);}
    public:
        static ScheduleUpdateLambda * create(std::function<void(const float deltaTime, bool & stop)> && lambda, int tag=0);
        virtual void execute(float deltaTime);
    protected:
        ScheduleUpdateLambda()=default;
        virtual ~ScheduleUpdateLambda()=default;
    };
    
    class ScheduleTimerLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void(const float deltaTime, const float progres, bool & stop)> _lambda=nullptr;
        std::function<void()> _finally=nullptr;
        virtual void update(float deltaTime) {execute(deltaTime);}
        bool _isDone=false;
        const float _duration;
        float _actionTime=0.f;
        virtual bool isDone(){return _isDone;}
        virtual void step(float dt){update(dt);}
    public:
        static ScheduleTimerLambda * create(const float duration, std::function<void(const float deltaTime, const float progres, bool & stop)> && lambda, std::function<void()> && finally=nullptr, int tag=0);
        virtual void execute(float deltaTime);
    protected:
        ScheduleTimerLambda(const float & duration): _duration(duration){}
        virtual ~ScheduleTimerLambda()=default;
    };
    
    namespace ProgressCurveFunctions{
        static float inline linearToEaseOut(const float progress){return 1.f-cosf(progress*M_PI_2);}
        static float inline linearToEaseIn(const float progress){return sinf(progress*M_PI_2);}
        float linearToBackInOut(float time, float multiplier=1.f, float m_fRate=1.5f);
    }
}}


#endif /* defined(__mcb__mcbCallLambda__) */
