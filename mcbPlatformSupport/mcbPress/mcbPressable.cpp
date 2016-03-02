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
    void Pressable::startListeningPresses(const int priority, const bool swallowsPresses, const std::set<Press::Type> & exceptionsToSwallows){PressDispatcher::sharedInstance()->_addPressable(this, priority, swallowsPresses, exceptionsToSwallows);}
    void Pressable::stopListeningPresses(){PressDispatcher::sharedInstance()->_removePressable(this);}
    bool Pressable::isListeningPresses() const{return PressDispatcher::sharedInstance()->_isListeningPresses(const_cast<Pressable *>(this));}
    Pressable::~Pressable(){
        stopListeningPresses();
    }
}}
