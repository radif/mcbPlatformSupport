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
    bool PressDispatcher::menuButtonClosesApp(){return _menuButtonClosesApp;}

    
    void PressDispatcher::dispatchPressBegan(const p_Press & press){
    
        switch (press->type) {
            case Press::TypeUpArrow:
                cocos2d::CCLog("TypeUpArrow");
                break;
            case Press::TypeDownArrow:
                cocos2d::CCLog("TypeDownArrow");
                break;
            case Press::TypeLeftArrow:
                cocos2d::CCLog("TypeLeftArrow");
                break;
            case Press::TypeRightArrow:
                cocos2d::CCLog("TypeRightArrow");
                break;
            case Press::TypeSelect:
                cocos2d::CCLog("TypeSelect");
                break;
            case Press::TypeMenu:
                cocos2d::CCLog("TypeMenu");
                break;
            case Press::TypePlayPause:
                cocos2d::CCLog("TypePlayPause");
                break;
                
                
            default:
                break;
        }
    }
    void PressDispatcher::dispatchPressChanged(const p_Press & press){}
    void PressDispatcher::dispatchPressEnded(const p_Press & press){
        switch (press->type) {
            case Press::TypeUpArrow:
                cocos2d::CCLog("TypeUpArrow");
                break;
            case Press::TypeDownArrow:
                cocos2d::CCLog("TypeDownArrow");
                break;
            case Press::TypeLeftArrow:
                cocos2d::CCLog("TypeLeftArrow");
                break;
            case Press::TypeRightArrow:
                cocos2d::CCLog("TypeRightArrow");
                break;
            case Press::TypeSelect:
                cocos2d::CCLog("TypeSelect");
                break;
            case Press::TypeMenu:
                cocos2d::CCLog("TypeMenu");
                break;
            case Press::TypePlayPause:
                cocos2d::CCLog("TypePlayPause");
                break;
                
                
            default:
                break;
        }
    }
    void PressDispatcher::dispatchPressCancelled(const p_Press & press){}

}}
