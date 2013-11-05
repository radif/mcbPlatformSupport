//
//  mcbVideoLayer.cpp
//  ColorDots
//
//  Created by Radif Sharafullin on 11/4/13.
//
//

#include "mcbVideoLayer.h"
using namespace cocos2d;
namespace mcb{namespace PlatformSupport{
    
    VideoLayer * VideoLayer::create(){
        auto retVal(new VideoLayer);
        if (retVal && retVal->init()) {
            retVal->autorelease();
        }else{
            CC_SAFE_DELETE(retVal);
            retVal=nullptr;
        }
        return retVal;
    }
    VideoLayer * VideoLayer::create(const std::string & videoPath){
        auto retVal(create());
        retVal->setVideoPath(videoPath);
        return retVal;
    }
    VideoLayer * VideoLayer::create(const std::string & videoPath, const cocos2d::CCRect & videoFrame){
        auto retVal(create());
        retVal->setVideoFrame(videoFrame);
        retVal->setVideoPath(videoPath);
        return retVal;
    }
    VideoLayer * VideoLayer::create(const cocos2d::CCRect & videoFrame){
        auto retVal(create());
        retVal->setVideoFrame(videoFrame);
        return retVal;
    }

}}