//
//  mcbRecursiveOpacity.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/11/13.
//
//

#ifndef __SoundSurfer__mcbRecursiveOpacity__
#define __SoundSurfer__mcbRecursiveOpacity__

#include "cocos2d.h"
#include <functional>

namespace mcb{namespace PlatformSupport{
    class RecursiveOpacity{
        void _setRecrusiveOpacity(cocos2d::CCNode * node, const GLubyte & opacity, const bool & absolute);
    public:
        void setRecrusiveOpacity(const GLubyte opacity, const bool absolute=false);
        
        void runFadeAction(const float duration, const GLubyte fromOpacity, const GLubyte toOpacity, const bool absolute=false, const std::function<void()> & completion=nullptr);
    };
}}


#endif /* defined(__SoundSurfer__mcbRecursiveOpacity__) */
