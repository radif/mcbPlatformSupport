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
    cocos2d::CCSequence * CallLambda::createWithDelay(const float delay, const std::function<void()>  & lambda, int tag){
        CallLambda *pRet = new CallLambda();
        if (pRet && pRet->initWithLambda(lambda)){
            pRet->autorelease();
            pRet->setTag(tag);
            auto retVal(cocos2d::CCSequence::createWithTwoActions(cocos2d::CCDelayTime::create(delay),pRet));
            retVal->setTag(tag);
            return retVal;
        }
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
    
    CallLambda * CallLambda::create(const std::function<void()> & lambda, int tag){
        CallLambda *pRet = new CallLambda();
        if (pRet && pRet->initWithLambda(lambda)){
            pRet->autorelease();
            pRet->setTag(tag);
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
    ScheduleTimerLambda * ScheduleTimerLambda::create(const float duration, std::function<void(const float deltaTime, const float progres, bool & stop)> && lambda, std::function<void()> && finally, int tag){
        ScheduleTimerLambda *retVal = new ScheduleTimerLambda(duration);
        if (retVal){
            retVal->_lambda=std::forward<decltype(lambda)>(lambda);
            retVal->_finally=std::forward<decltype(finally)>(finally);
            retVal->setTag(tag);
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
                if (_finally)
                    _finally();
                return;
            }
            _lambda(deltaTime, progress ,_isDone);
        }
    }
    
    
    //ScheduleUpdateLambda
    ScheduleUpdateLambda * ScheduleUpdateLambda::create(std::function<void(const float deltaTime, bool & stop)> && lambda, int tag){
        ScheduleUpdateLambda *retVal = new ScheduleUpdateLambda();
        if (retVal){
            retVal->_lambda=std::forward<decltype(lambda)>(lambda);
            retVal->setTag(tag);
            retVal->autorelease();
            return retVal;
        }
        CC_SAFE_DELETE(retVal);
        return nullptr;
    }
    
    void ScheduleUpdateLambda::execute(float deltaTime){
        if (_lambda){
            _lambda(deltaTime,_isDone);
        }
    }
    namespace ProgressCurveFunctions{
        float linearToBackInOut(float progress, float multiplier, float rate){
            auto innerUpdateL([=](float time)->float{
                const float overshoot(1.70158f*1.525f*multiplier);
                time=time*2.f;
                if (time<1.f)
                    return (time*time*((overshoot+1.f)*time-overshoot))*.5f;
                
                time=time-2.f;
                return (time*time*((overshoot+1.f)*time+overshoot))*.5f+1.f;
            });
            progress*=2.f;
            if (progress<1.f)
                return innerUpdateL(0.5f*powf(progress, rate));
            
            return innerUpdateL(1.f-.5f*powf(2.f-progress, rate));
        }
        float linearToBackIn(float preogress, float rate){
            auto innerUpdateL([=](float time)->float{
                float overshoot(1.70158f);
                return time*time*((overshoot+1)*time-overshoot);
            });
            preogress*=2.f;
            if (preogress<1.f)
                return innerUpdateL(0.5f*powf(preogress, rate));
            
            return innerUpdateL(1.f-.5f*powf(2.f-preogress, rate));
        }
        float linearToBackOut(float progress, float rate){
            auto innerUpdateL([=](float time)->float{
                float overshoot = 1.70158f;
                time=time-1.f;
                return time*time*((overshoot+1.f)*time+overshoot)+1.f;
            });
            progress*=2.f;
            if (progress<1.f)
                return innerUpdateL(0.5f*powf(progress, rate));
            
            return innerUpdateL(1.f-.5f*powf(2.f-progress, rate));
        }
        
        // Bezier cubic formula:
        //    ((1 - t) + t)3 = 1
        // Expands to°≠
        //   (1 - t)3 + 3t(1-t)2 + 3t2(1 - t) + t3 = 1
        static inline float bezierat(const float a, const float b, const float c, const float d, const float t){
            return (powf(1-t,3) * a +
                    3*t*(powf(1-t,2))*b +
                    3*powf(t,2)*(1-t)*c +
                    powf(t,3)*d );
        }
        
        pBezier Bezier::create(){return std::make_shared<Bezier>();}
        pBezier Bezier::create(const cocos2d::CCPoint & startPos, const bezier_configs_t & configs){
            auto retVal(create());
            retVal->_startPosition=startPos;
            retVal->_configs=configs;
            return retVal;
        }
        void Bezier::addConfig(const BezierConfig & config){
            _configs.emplace_back(config);
        }
        void Bezier::addConfig(const cocos2d::CCPoint & pt1, const cocos2d::CCPoint & pt2, const cocos2d::CCPoint & pt3){
            _configs.emplace_back(BezierConfig{pt1,pt2,pt3});
        }
        void Bezier::setConfigs(const bezier_configs_t & configs){
            _configs=configs;
        }
        cocos2d::CCPoint Bezier::pointAt(float progress){
            const float configIndexF(_configs.size()*progress);
            const int configIndex(configIndexF);
            const float time(configIndexF-configIndex);
            const BezierConfig & conf(_configs.at(configIndex));
            
            //get the step
            const cocos2d::CCPoint startPos(configIndex==0? _startPosition: _configs.at(configIndex-1).endPos);
            
            const float xa(startPos.x);
            const float xb(conf.controlPt1.x);
            const float xc(conf.controlPt2.x);
            const float xd(conf.endPos.x);
            
            const float ya(startPos.y);
            const float yb(conf.controlPt1.y);
            const float yc(conf.controlPt2.y);
            const float yd(conf.endPos.y);
            
            return {bezierat(xa, xb, xc, xd, time), bezierat(ya, yb, yc, yd, time)};
        }

        
    }
}}
