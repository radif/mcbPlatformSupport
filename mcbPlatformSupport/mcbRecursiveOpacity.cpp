//
//  mcbRecursiveOpacity.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/11/13.
//
//

#include "mcbRecursiveOpacity.h"
#include "mcbPlatformSupport.h"
#include "mcbPlatformSupportFunctions.h"
#include "mcbCallLambda.h"

using namespace cocos2d;
namespace mcb{namespace PlatformSupport{
    void RecursiveOpacity::setRecrusiveOpacity(const GLubyte opacity, const bool absolute){
        auto node(Functions::convertToNodeType<cocos2d::CCNode *>(this));
        if (node)
            _setRecrusiveOpacity(node, opacity, absolute);
    }
    void RecursiveOpacity::_setRecrusiveOpacity(cocos2d::CCNode * node, const GLubyte & opacity, const bool & absolute){
        //children
        CCArray * children(node->getChildren());
        if (children && children->count()) {
            mcbForEachBegin(cocos2d::CCNode *, n, children)
            _setRecrusiveOpacity(n, opacity, absolute);
            mcbForEachEnd
        }
        auto sprite(Functions::convertToNodeType<cocos2d::CCSprite *>(node));
        if (sprite) {
            //opacity
            if (absolute)
                sprite->setOpacity(opacity);
            else{
                GLubyte o(sprite->getOpacity());
                const float multiplier(o/255.f);
                sprite->setOpacity(opacity*multiplier);
            }
        }
    
    }
    void RecursiveOpacity::runFadeAction(const float duration, const GLubyte fromOpacity, const GLubyte toOpacity, const bool absolute, const std::function<void()> & completion){
        auto node(Functions::convertToNodeType<cocos2d::CCNode *>(this));
        if (node) {
            int delta(toOpacity-fromOpacity);
            node->runAction(PlatformSupport::create_scheduleLambda(0.f, [=](float dt, float * pActionTime, bool &stop){
                (*pActionTime)+=dt;
                if (*pActionTime>=duration) {
                    setRecrusiveOpacity(toOpacity, absolute);
                    if (completion)
                        completion();
                    stop=true;
                }
                float progress(*pActionTime/duration);
                //float curvedProgress(1.f-cos(progress*M_PI_2));
                setRecrusiveOpacity(fromOpacity+delta*progress, absolute);
            }));
            
        }

        
        

    }

}}
