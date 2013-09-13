//
//  mcbViewBuilder.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/12/13.
//
//

#ifndef __SoundSurfer__mcbViewBuilder__
#define __SoundSurfer__mcbViewBuilder__

#include "cocos2d.h"
#include "mcbPath.h"

namespace mcb{namespace PlatformSupport{
    class Path;
    class ViewBuilder : protected virtual Path{
        virtual void _populateChildren(cocos2d::CCArray * children, cocos2d::CCNode * parent);
         cocos2d::CCNode * const _thisNode;
        
        struct ButtonEventReceiver : public cocos2d::CCObject{
            typedef std::function<void(cocos2d::CCObject * button, const int & tag)> callback_t;
            void _buttonWithTagPressed(cocos2d::CCObject * button);
            callback_t buttonHandler=nullptr;
            ButtonEventReceiver(callback_t callback);
            virtual ~ButtonEventReceiver();
        };
        ButtonEventReceiver * const _buttonEventReceiver;
    protected:
        std::map<std::string, std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)>> _generators;
        void setFactoryForKey(const std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)> & lambda, const std::string & key);
        std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)> factoryForKey(const std::string & key);
        virtual void buildViewWithSceneData(cocos2d::CCDictionary * sceneData);
        virtual void buildViewWithData(cocos2d::CCDictionary * data);
        virtual void buttonWithTagPressed(cocos2d::CCObject * button, int tag){}
        ViewBuilder(cocos2d::CCNode *thisNode);
        virtual ~ViewBuilder();
    };
}}

#endif /* defined(__SoundSurfer__mcbViewBuilder__) */
