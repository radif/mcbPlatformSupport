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
    void setRecursiveOpacity(cocos2d::CCNode * node, const GLubyte & opacity, const bool & absolute){
        //children
        CCArray * children(node->getChildren());
        if (children && children->count()) {
            mcbForEachBegin(cocos2d::CCNode *, n, children)
            setRecursiveOpacity(n, opacity, absolute);
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
    void RecursiveOpacity::setOpacityRecursively(const GLubyte opacity, const bool absolute){
        auto node(Functions::convertToNodeType<cocos2d::CCNode *>(this));
        if (node)
            setRecursiveOpacity(node, opacity, absolute);
    }
    
    void RecursiveOpacity::runFadeAction(const float duration, const GLubyte fromOpacity, const GLubyte toOpacity, const bool absolute, const std::function<void()> & completion){
        auto node(Functions::convertToNodeType<cocos2d::CCNode *>(this));
        if (node) {
            int delta(toOpacity-fromOpacity);
            node->runAction(PlatformSupport::create_scheduleLambda(0.f, [=](float dt, float * pActionTime, bool &stop){
                (*pActionTime)+=dt;
                if (*pActionTime>=duration) {
                    setOpacityRecursively(toOpacity, absolute);
                    if (completion)
                        completion();
                    stop=true;
                }
                float progress(*pActionTime/duration);
                //float curvedProgress(1.f-cos(progress*M_PI_2));
                setOpacityRecursively(fromOpacity+delta*progress, absolute);
            }));
            
        }

        
        

    }

}}
