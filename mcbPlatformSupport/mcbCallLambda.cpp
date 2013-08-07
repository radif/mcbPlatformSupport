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
    cocos2d::CCSequence * CallLambda::createWithDelay(float delay, std::function<void()> lambda){
        CallLambda *pRet = new CallLambda();
        if (pRet && pRet->initWithLambda(lambda)){
            pRet->autorelease();
            return cocos2d::CCSequence::createWithTwoActions(cocos2d::CCDelayTime::create(delay),pRet);
        }
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
    
    CallLambda * CallLambda::create(std::function<void()> lambda){
        CallLambda *pRet = new CallLambda();
        if (pRet && pRet->initWithLambda(lambda)){
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
    bool CallLambda::initWithLambda(const std::function<void()> & lambda){
        _lambda=std::move(lambda);
        return true;
    }
}}
