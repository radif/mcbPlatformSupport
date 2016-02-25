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
    void Pressable::startListeningPresses(const int priority, const bool swallowsPresses){PressDispatcher::sharedInstance()->_addPressable(this, priority, swallowsPresses);}
    void Pressable::stopListeningPresses(){PressDispatcher::sharedInstance()->_removePressable(this);}
    bool Pressable::isListeningPresses() const{return PressDispatcher::sharedInstance()->_isListeningPresses(const_cast<Pressable *>(this));}
    Pressable::~Pressable(){
        stopListeningPresses();
    }
}}
