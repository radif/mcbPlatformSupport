//
//  mcbPressDispatcher.cpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/14/16.
//
//

#include "mcbPressDispatcher.hpp"
namespace mcb{namespace PlatformSupport{
    void PressDispatcher::init(){
        
    }
    
    void PressDispatcher::setMenuButtonClosesApp(const bool menuButtonClosesApp){
        _menuButtonClosesApp=menuButtonClosesApp;
    }
    bool PressDispatcher::menuButtonClosesApp() const{return _menuButtonClosesApp;}
    
    

    void PressDispatcher::setRemoteControlInteractionEnabled(const bool remoteControlInteractionEnabled){
        _remoteControlInteractionEnabled=remoteControlInteractionEnabled;
    }
    bool PressDispatcher::remoteControlInteractionEnabled() const{return _remoteControlInteractionEnabled;}
    
#pragma mark -
    
    void PressDispatcher::dispatchPressBegan(const p_Press & press){
        if(_remoteControlInteractionEnabled){
            auto pressables(_pressables);
            std::for_each(pressables.begin(), pressables.end(), [&](Pressable *pressable){pressable->pressBegan(press);});
        }
    }
    void PressDispatcher::dispatchPressChanged(const p_Press & press){
        if(_remoteControlInteractionEnabled){
            auto pressables(_pressables);
            std::for_each(pressables.begin(), pressables.end(), [&](Pressable *pressable){pressable->pressChanged(press);});
        }
    }
    void PressDispatcher::dispatchPressEnded(const p_Press & press){
        if(_remoteControlInteractionEnabled){
            auto pressables(_pressables);
            std::for_each(pressables.begin(), pressables.end(), [&](Pressable *pressable){pressable->pressEnded(press);});
        }
    }
    void PressDispatcher::dispatchPressCancelled(const p_Press & press){
        if(_remoteControlInteractionEnabled){
            auto pressables(_pressables);
            std::for_each(pressables.begin(), pressables.end(), [&](Pressable *pressable){pressable->pressCancelled(press);});
        }
    }
    
#pragma mark -
#pragma mark add, remove pressables;
    void PressDispatcher::_addPressable(Pressable * pressable){
        _pressables.emplace(pressable);
    }
    void PressDispatcher::_removePressable(Pressable * pressable){
        auto it(_pressables.find(pressable));
        if (it!=_pressables.end())
            _pressables.erase(it);
    }

}}
