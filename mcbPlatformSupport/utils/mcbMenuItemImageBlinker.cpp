//
//  mcbMenuItemImageBlinker.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 10/12/13.
//
//

#include "mcbMenuItemImageBlinker.h"
#include "mcbCallLambda.h"
#include "mcbAudioPlayer.h"

using namespace cocos2d;
namespace mcb{ namespace PlatformSupport{
    static MenuItemImageBlinkerContext * _sharedInstance(nullptr);
    MenuItemImageBlinkerContext * MenuItemImageBlinkerContext::sharedInstance(){
        static std::once_flag onceFlag;
        std::call_once(onceFlag,[=](){
            _sharedInstance=new MenuItemImageBlinkerContext;
        });
        return _sharedInstance;
    }
    
    
    static const int kSpriteBlinkingActionTag(1198163411);
    void MenuItemImageBlinker::activate(){
        if (_blinkingEnabled)
            _mcbActivate([=](){super::activate();});
        else
            super::activate();
        if(!_selectSoundPath.empty())
            PlatformSupport::AudioPlayer::playOneShot(_selectSoundPath);
    }
    void MenuItemImageBlinker::_mcbActivate(const std::function<void()> & completion){
        CCDirector::sharedDirector()->getTouchDispatcher()->setDispatchEvents(false);
        stopActionByTag(kSpriteBlinkingActionTag);
        _setBlinking(true, _blinkingColorsSelected, _blinkingDelaySelected);
        
        runAction(mcb::PlatformSupport::CallLambda::createWithDelay(_selectedBlinkingDuration, [=](){
            CCDirector::sharedDirector()->getTouchDispatcher()->setDispatchEvents(true);
            if (completion)
                completion();
            _setBlinking(_blinksOnIdle && _blinkingEnabled, _blinkingColors, _blinkingDelay);
        }));
    }
    void MenuItemImageBlinker::_setBlinking(bool blinking, const std::vector<cocos2d::ccColor3B> colors, float delay){
        stopActionByTag(kSpriteBlinkingActionTag);
        if (blinking) {
            float totalTime(0);
            int colorIndex(0);
            auto nextColorL([=]()mutable ->cocos2d::ccColor3B{
                auto retVal(colors.at(colorIndex));
                colorIndex++;
                if (colorIndex>=colors.size())
                    colorIndex=0;
                return retVal;
            });
            setColor(nextColorL());
            runAction(mcb::PlatformSupport::ScheduleUpdateLambda::create([=](const float dt, bool &stop)mutable{
                totalTime+=dt;
                if (totalTime>delay) {
                    totalTime=0;
                    setColor(nextColorL());
                }
            }, kSpriteBlinkingActionTag));
        }else
            setColor(ccWHITE);
    }
    void MenuItemImageBlinker::setBlinkingEnabled(bool blinkingEnabled){
        _blinkingEnabled=blinkingEnabled;
        _setBlinking(_blinksOnIdle && _blinkingEnabled, _blinkingColors, _blinkingDelay);
    }
    void MenuItemImageBlinker::setBlinksOnIdle(bool blinksOnIdle){
        _blinksOnIdle=blinksOnIdle;
        _setBlinking(_blinksOnIdle && _blinkingEnabled, _blinkingColors, _blinkingDelay);
    }
    void MenuItemImageBlinker::setupWithBlinkingContext(){
        MenuItemImageBlinkerContext * context(MenuItemImageBlinkerContext::sharedInstance());
        setSpriteBlinkingDelay(context->spriteBlinkingDelay());
        setSelectedSpriteBlinkingDelay(context->selectedSpriteBlinkingDelay());
        setSpriteBlinkingColors(context->spriteBlinkingColors());
        setSelectedSpriteBlinkingColors(context->selectedSpriteBlinkingColors());
    }
}}