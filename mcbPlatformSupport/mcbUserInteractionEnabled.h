//
//  mcbUserInteractionEnabled.h
//  mcb
//
//  Created by Radif Sharafullin on 6/15/13.
//
//

#ifndef __mcb__mcbUserInteractionEnabled__
#define __mcb__mcbUserInteractionEnabled__

#include "cocos2d.h"

namespace mcb{namespace PlatformSupport{
    class UserInteractionEnabled{
        int _userInteractionEnabledCounter=1;
    public:
        virtual void setUserInteractionEnabled(bool enabled);
        virtual bool getUserInteractionEnabled(){return !((bool)_userInteractionEnabledCounter);};
    protected:
        //subclass this to implement your custom implementation of disabling interaction
        //don't call these functions directly, instead call setUserInteractionEnabled
        virtual void enableInteraction()=0;
        virtual void disableInteraction()=0;
    };
}}
#endif /* defined(__mcb__mcbUserInteractionEnabled__) */
