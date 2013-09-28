//
//  mcbCallLambda.cpp
//  mcb
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#include "mcbCallLambda.h"

namespace mcb{namespace PlatformSupport{
    
    //mcbCallLambda
    
    void CallLambda::execute(){
        if (_lambda)
            _lambda();
    }
    cocos2d::CCSequence * CallLambda::createWithDelay(const float delay, const std::function<void()>  & lambda){
        CallLambda *pRet = new CallLambda();
        if (pRet && pRet->initWithLambda(lambda)){
            pRet->autorelease();
            return cocos2d::CCSequence::createWithTwoActions(cocos2d::CCDelayTime::create(delay),pRet);
        }
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
    
    CallLambda * CallLambda::create(const std::function<void()> & lambda){
        CallLambda *pRet = new CallLambda();
        if (pRet && pRet->initWithLambda(lambda)){
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
    bool CallLambda::initWithLambda(const std::function<void()> & lambda){
        _lambda=std::forward<decltype(lambda)>(lambda);
        return true;
    }
    
    
    //ScheduleTimerLambda
    ScheduleTimerLambda * ScheduleTimerLambda::create(const float duration, std::function<void(const float deltaTime, const float progres, bool & stop)> && lambda){
        ScheduleTimerLambda *retVal = new ScheduleTimerLambda(duration);
        if (retVal){
            retVal->_lambda=std::forward<decltype(lambda)>(lambda);
            retVal->autorelease();
            return retVal;
        }
        CC_SAFE_DELETE(retVal);
        return nullptr;
    }
    
    void ScheduleTimerLambda::execute(float deltaTime){
        if (_lambda){
            _actionTime+=deltaTime;
            float progress(_actionTime/_duration);
            if (progress>=1.f) {
                _isDone=true;
                return;
            }
            _lambda(deltaTime, progress ,_isDone);
        }
    }
    
}}
