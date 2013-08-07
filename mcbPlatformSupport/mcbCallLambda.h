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
        static CallLambda * create(std::function<void()> lambda);
        static cocos2d::CCSequence * createWithDelay(float delay, std::function<void()> lambda);
        virtual bool initWithLambda(const std::function<void()> & lambda);
        virtual void execute();
    protected:
        CallLambda(){}
        virtual ~CallLambda(){}
    };
    
    template <typename T>
    class ScheduleLambda : public cocos2d::CCActionInstant{
        typedef cocos2d::CCActionInstant super;
        std::function<void(float deltaTime, T *userData , bool & stop)> _lambda=nullptr;
        virtual void update(float deltaTime) {execute(deltaTime);}
        bool _isDone=false;
        virtual bool isDone(){return _isDone;}
        virtual void step(float dt){update(dt);}
        T _userData;
    public:
        static ScheduleLambda * create(T && userData, std::function<void(float deltaTime, T *userData , bool & stop)> &&lambda){
            ScheduleLambda *retVal = new ScheduleLambda();
            if (retVal){
                retVal->_userData=std::forward<T>(userData);
                retVal->_lambda=std::move(lambda);
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
        ScheduleLambda(){}
        virtual ~ScheduleLambda(){}
    };
    template <typename T>
    ScheduleLambda<T> * create_scheduleLambda(T userData, std::function<void(float deltaTime, decltype(userData) *userData , bool & stop)> && lambda){
        return ScheduleLambda<T>::create(std::move(userData), std::move(lambda));
    }
    
    //TODO:
    //create_scheduleLambda_progress //take easein/out params, auto stop
    
    //0-1
    //float linearToEaseIn(const float & progress){return sin(progress*M_PI_2);}
    //float linearToEaseOut(const float & progress){return sin(progress*M_PI_2);}
    
    
    //float progress(*pActionTime/duration);

    
    //auto stops on progress
    //
    
//    ScheduleLambda<T> * create_scheduleLambda_progress(T userData, std::function<void(float deltaTime, decltype(userData) *userData , bool & stop, bool isLastCall)> && lambda, scheduleLambdaCurve=scheduleLambdaCurveLinear){
//        return ScheduleLambda<T>::create(std::move(userData), std::move(lambda));
//    }
    
}}


#endif /* defined(__mcb__mcbCallLambda__) */
