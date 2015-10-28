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
        public:
        struct ButtonEventReceiver : public cocos2d::CCObject{
            typedef std::function<void(cocos2d::CCObject * button, const int & tag)> callback_t;
            void _buttonWithTagPressed(cocos2d::CCObject * button);
            callback_t buttonHandler=nullptr;
            ButtonEventReceiver(callback_t callback);
            virtual ~ButtonEventReceiver();
        };
        class OriginalDictionaryBinder{
            class NodeToDictionaryBinding{
                cocos2d::CCDictionary * _originalData=nullptr;
                cocos2d::CCNode * _node=nullptr;
            public:
                NodeToDictionaryBinding(cocos2d::CCDictionary * originalData, cocos2d::CCNode * node);
                virtual ~NodeToDictionaryBinding();
                cocos2d::CCNode * node() const;
                cocos2d::CCDictionary * data() const;
            };
            typedef std::shared_ptr<NodeToDictionaryBinding> pNodeToDictionaryBinding;
            std::map<cocos2d::CCNode *, pNodeToDictionaryBinding> _binders;
        public:
            void setOriginalDictionaryToDerivativeNodeNode(cocos2d::CCDictionary * originalData, cocos2d::CCNode * node);
            std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> allChildrenWithOriginalData() const;
            std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> childrenWithOriginalDataForTag(const int tag) const;
            cocos2d::CCDictionary * originalDictionaryForNode(cocos2d::CCNode * node) const;
            bool removeChildNodeWithOriginalData(cocos2d::CCNode * node, bool removeFromParent=true, bool cleanup=true);
        } _originalDictionaryBinder;
        
    private:
        ButtonEventReceiver * const _buttonEventReceiver;
    protected:
        ButtonEventReceiver*buttonEventReceiver(){return _buttonEventReceiver;}
        virtual void initGenerators();//override it to change the default generators. call super to keep the existing ones. Otherwise, use functions below to add factories
        
        std::map<std::string, std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)>> _generators;
        void setFactoryForKey(const std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)> & lambda, const std::string & key);
        template<class C>void setMCBNodeFactoryForKey(const std::string & key, const std::function<void(C *)> & instantiationHook=nullptr){setFactoryForKey([=](cocos2d::CCDictionary *data)->cocos2d::CCNode *{C* retVal(C::createInstance(_localPath, data));if(instantiationHook)instantiationHook(retVal);return retVal;}, key);}

        std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)> factoryForKey(const std::string & key);
        
        std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> allChildrenWithOriginalData() const;
        std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> childrenWithOriginalDataForTag(const int tag) const;
        cocos2d::CCDictionary * originalDataForChildNode(cocos2d::CCNode * node) const;
        //this needs to be called in order to remove sprites created by view builder to prevent dead pointers in the bindlers and crashing on methids such as childrenWithOriginalDataForTag();
        bool removeChildNodeWithOriginalData(cocos2d::CCNode * node, bool removeFromParent=true, bool cleanup=true);
        
        
        virtual void buildViewWithSceneData(cocos2d::CCDictionary * sceneData);
        virtual void buildViewWithData(cocos2d::CCDictionary * data);
        virtual void buttonWithTagPressed(cocos2d::CCMenuItem * button, int tag){}
        virtual void buttonWithContextPressed(cocos2d::CCMenuItem * button, cocos2d::CCDictionary * context, int tag){}
        
        
        ViewBuilder(cocos2d::CCNode *thisNode);//pass from a subclass. Subclass has to be a CCNode * instance
        virtual ~ViewBuilder();
    };
}}

#endif /* defined(__SoundSurfer__mcbViewBuilder__) */
