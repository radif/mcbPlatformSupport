//
//  mcbMenuItemImageBlinker.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 10/12/13.
//
//

#ifndef __SoundSurfer__mcbMenuItemImageBlinker__
#define __SoundSurfer__mcbMenuItemImageBlinker__

#include "cocos2d.h"
namespace mcb{ namespace PlatformSupport{
    class MenuItemImageBlinkerContext{
        std::vector<cocos2d::ccColor3B> _blinkingColors;
        std::vector<cocos2d::ccColor3B> _blinkingColorsSelected;
        float _blinkingDelay=0.f;
        float _blinkingDelaySelected=0.f;
    public:
        static MenuItemImageBlinkerContext * sharedInstance();
        
        void setSpriteBlinkingColors(const std::vector<cocos2d::ccColor3B> colors){_blinkingColors=colors;}
        void setSelectedSpriteBlinkingColors(const std::vector<cocos2d::ccColor3B> colors){_blinkingColorsSelected=colors;}
        void setSpriteBlinkingDelay(const float delay){_blinkingDelay=delay;}
        void setSelectedSpriteBlinkingDelay(const float delay){_blinkingDelaySelected=delay;}
        
        const std::vector<cocos2d::ccColor3B> & spriteBlinkingColors(){return _blinkingColors;}
        const std::vector<cocos2d::ccColor3B> & selectedSpriteBlinkingColors(){return _blinkingColorsSelected;}
        
        float spriteBlinkingDelay(){return _blinkingDelay;}
        float selectedSpriteBlinkingDelay(){return _blinkingDelaySelected;}
        
        
    };
    class MenuItemImageBlinker : public cocos2d::CCMenuItemImage{
        typedef cocos2d::CCMenuItemImage super;
        
        std::string _selectSoundPath;
        
        bool _blinkingEnabled=true;
        bool _blinksOnIdle=true;
        
        std::vector<cocos2d::ccColor3B> _blinkingColors;
        std::vector<cocos2d::ccColor3B> _blinkingColorsSelected;

        float _blinkingDelay=0.f;
        float _blinkingDelaySelected=0.f;
        void _setBlinking(bool blinking, const std::vector<cocos2d::ccColor3B> colors, float delay);
        void _mcbActivate(const std::function<void()> & completion);
        float _selectedBlinkingDuration=.4f;
    public:
        static MenuItemImageBlinker * create(const char *normalImage, const char *selectedImage){
            MenuItemImageBlinker *retVal(new MenuItemImageBlinker());
            if (retVal && retVal->initWithNormalImage(normalImage, selectedImage, nullptr, nullptr, nullptr)){
                retVal->autorelease();
                return retVal;
            }
            CC_SAFE_DELETE(retVal);
            return nullptr;
        }
        virtual void activate() override;
        void setBlinkingEnabled(bool blinkingEnabled);
        void setBlinksOnIdle(bool blinksOnIdle);
        void setSpriteBlinkingColors(const std::vector<cocos2d::ccColor3B> colors){_blinkingColors=colors;}
        void setSelectedSpriteBlinkingColors(const std::vector<cocos2d::ccColor3B> colors){_blinkingColorsSelected=colors;}
        void setSpriteBlinkingDelay(const float delay){_blinkingDelay=delay;}
        void setSelectedSpriteBlinkingDelay(const float delay){_blinkingDelaySelected=delay;}
        void setSelectSoundPath(const std::string soundPath){_selectSoundPath=soundPath;}
        void setSelectedBlinkingDuration(const float duration){_selectedBlinkingDuration=duration;}
        
        void setupWithBlinkingContext();
        
        const std::vector<cocos2d::ccColor3B> & spriteBlinkingColors(){return _blinkingColors;}
        const std::vector<cocos2d::ccColor3B> & selectedSpriteBlinkingColors(){return _blinkingColorsSelected;}

        float spriteBlinkingDelay(){return _blinkingDelay;}
        float selectedSpriteBlinkingDelay(){return _blinkingDelaySelected;}
        const std::string & selectSoundPath(){return _selectSoundPath;}
        float selectedBlinkingDuration(){return _selectedBlinkingDuration;}
        
        
    protected:
    };
}}

#endif /* defined(__SoundSurfer__mcbMenuItemImageBlinker__) */
