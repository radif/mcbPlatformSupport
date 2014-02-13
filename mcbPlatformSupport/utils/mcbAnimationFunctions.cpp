//
//  mcbAnimationFunctions.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/12/13.
//
//

#include "mcbAnimationFunctions.h"
using namespace cocos2d;
namespace mcb{ namespace PlatformSupport{
    mcbEaseBackInOut::mcbEaseBackInOut(float overshot):_overshot(overshot){}
    mcbEaseBackInOut::~mcbEaseBackInOut(){}

    void mcbEaseBackInOut::update(float time){
        float overshoot(1.70158f * 1.525f *_overshot);
        time = time * 2;
        if (time < 1){
            m_pInner->update((time * time * ((overshoot + 1) * time - overshoot)) / 2);
        }
        else{
            time = time - 2;
            m_pInner->update((time * time * ((overshoot + 1) * time + overshoot)) / 2 + 1);
        }
    }
    
    /** creates the action */
    mcbEaseBackInOut* mcbEaseBackInOut::create(CCActionInterval* pAction, float overshot){
        mcbEaseBackInOut *pRet(new mcbEaseBackInOut(overshot));
        if (pRet){
            if (pRet->initWithAction(pAction))
                pRet->autorelease();
            else
                CC_SAFE_RELEASE_NULL(pRet);
        }
        return pRet;
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
        const CCPoint startPos(configIndex==0? _startPosition: _configs.at(configIndex-1).endPos);
        
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

}}
