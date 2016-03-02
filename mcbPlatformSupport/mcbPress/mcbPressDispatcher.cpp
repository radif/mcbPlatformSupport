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
            
            bool swallowsPress(false);
            std::set<Press::Type> exceptionsToSwallows;
            for (Pressable *pressable : pressables) {
                swallowsPress=pressable->__swallowsPresses;//local copy
                if(swallowsPress)//local copy set only if swallows
                    exceptionsToSwallows=pressable->__exceptionsToSwallows;
                pressable->pressBegan(press);//possibly destructive to the pressable and changes the __pressables
                if (swallowsPress){
                    auto it(exceptionsToSwallows.find(press->type));
                    if (it==exceptionsToSwallows.end())
                        break;
                }
                
            }
        }
    }
    void PressDispatcher::dispatchPressChanged(const p_Press & press){
        if(_remoteControlInteractionEnabled){
            auto pressables(_pressables);

            bool swallowsPress(false);
            std::set<Press::Type> exceptionsToSwallows;
            for (Pressable *pressable : pressables) {
                swallowsPress=pressable->__swallowsPresses;//local copy
                if(swallowsPress)//local copy set only if swallows
                    exceptionsToSwallows=pressable->__exceptionsToSwallows;
                pressable->pressChanged(press);//possibly destructive to the pressable and changes the __pressables
                if (swallowsPress){
                    auto it(exceptionsToSwallows.find(press->type));
                    if (it==exceptionsToSwallows.end())
                        break;
                }
            }
        }
    }
    void PressDispatcher::dispatchPressEnded(const p_Press & press){
        if(_remoteControlInteractionEnabled){
            auto pressables(_pressables);
            
            bool swallowsPress(false);
            std::set<Press::Type> exceptionsToSwallows;
            for (Pressable *pressable : pressables) {
                swallowsPress=pressable->__swallowsPresses;//local copy
                if(swallowsPress)//local copy set only if swallows
                    exceptionsToSwallows=pressable->__exceptionsToSwallows;
                pressable->pressEnded(press);//possibly destructive to the pressable and changes the __pressables
                if (swallowsPress){
                    auto it(exceptionsToSwallows.find(press->type));
                    if (it==exceptionsToSwallows.end())
                        break;
                }
            }
        }
    }
    void PressDispatcher::dispatchPressCancelled(const p_Press & press){
        if(_remoteControlInteractionEnabled){
            auto pressables(_pressables);
            
            bool swallowsPress(false);
            std::set<Press::Type> exceptionsToSwallows;
            for (Pressable *pressable : pressables) {
                swallowsPress=pressable->__swallowsPresses;//local copy
                if(swallowsPress)//local copy set only if swallows
                    exceptionsToSwallows=pressable->__exceptionsToSwallows;
                pressable->pressCancelled(press);//possibly destructive to the pressable and changes the __pressables
                if (swallowsPress){
                    auto it(exceptionsToSwallows.find(press->type));
                    if (it==exceptionsToSwallows.end())
                        break;
                }
            }
        }
    }
    
#pragma mark -
#pragma mark add, remove pressables;
    void PressDispatcher::_addPressable(Pressable * pressable, const int priority, const bool swallowsPresses, const std::set<Press::Type> & exceptionsToSwallows){
        pressable->__priority=priority;
        pressable->__swallowsPresses=swallowsPresses;
        pressable->__exceptionsToSwallows=exceptionsToSwallows;
        _pressables.emplace_back(pressable);
        std::sort(_pressables.begin(), _pressables.end(), [](Pressable *p1, Pressable *p2){return p1->__priority>p2->__priority;});
    }
    void PressDispatcher::_removePressable(Pressable * pressable){
       // _pressables.erase(std::remove(_pressables.begin(), _pressables.end(), pressable), _pressables.end());
        auto it(std::find(_pressables.begin(), _pressables.end(), pressable));
        if (it!=_pressables.end())
            _pressables.erase(it);
    }
    bool PressDispatcher::_isListeningPresses(Pressable * pressable){
        auto it(std::find(_pressables.begin(), _pressables.end(), pressable));
        return it!=_pressables.end();
    }
}}
