//
//  mcbPressable.cpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/14/16.
//
//

#include "mcbPressable.hpp"
#include "mcbPressDispatcher.hpp"

namespace mcb{namespace PlatformSupport{
    void Pressable::startListeningPresses(){PressDispatcher::sharedInstance()->_addPressable(this);}
    void Pressable::stopListeningPresses(){PressDispatcher::sharedInstance()->_removePressable(this);}
    Pressable::~Pressable(){
        stopListeningPresses();
    }
}}
