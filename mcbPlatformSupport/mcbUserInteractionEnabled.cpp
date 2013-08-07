//
//  mcbUserInteractionEnabled.cpp
//  mcb
//
//  Created by Radif Sharafullin on 6/15/13.
//
//

#include "mcbUserInteractionEnabled.h"
using namespace cocos2d;

namespace mcb{namespace PlatformSupport{
#pragma mark userInteractionHandlers
    void UserInteractionEnabled::setUserInteractionEnabled(bool enabled){
        if (enabled) {
            _userInteractionEnabledCounter--;
            if (_userInteractionEnabledCounter == 0)
                enableInteraction();
            
            
            if (_userInteractionEnabledCounter < 0) _userInteractionEnabledCounter = 0;
        }else{
            _userInteractionEnabledCounter++;
            if (_userInteractionEnabledCounter == 1)
                disableInteraction();
        }
    }
}}