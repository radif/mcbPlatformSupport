//
//  mcbViewBuilder.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 9/12/13.
//
//

#include "mcbViewBuilder.h"
#include "mcbPlatformSupportFunctions.h"
#include "mcbNumber.h"
#include "cocos-ext.h"
#include "mcbAccelerometerParallaxNode.h"
#include "mcbVideoLayer.h"

using namespace cocos2d;
namespace mcb{namespace PlatformSupport{
    ViewBuilder::ViewBuilder(cocos2d::CCNode * thisNode):_thisNode(thisNode), _buttonEventReceiver(new ButtonEventReceiver([=](cocos2d::CCObject * button, const int & tag){buttonWithTagPressed((cocos2d::CCMenuItem *)button, tag);buttonWithContextPressed((cocos2d::CCMenuItem *)button, (CCDictionary *) originalDataForChildNode(((cocos2d::CCMenuItem *)button)->getParent())->objectForKey(Constants::kContextString), tag);})){
        assert(_thisNode);
        assert(dynamic_cast<CCNode *>(_thisNode));

        initGenerators();
    }
    ViewBuilder::~ViewBuilder(){
        _buttonEventReceiver->release();
    }
#pragma mark view builder
    void ViewBuilder::initGenerators(){
        _generators["node"]=[=](CCDictionary * data)->CCNode *{
            cocos2d::CCNode * node(cocos2d::CCNode::create());
            Functions::setNodeProperties(node, data);
            return node;
            
        };
        
        _generators["ui-parallax-node"]=[=](CCDictionary * data)->CCNode *{
            AccelerometerParallaxNode * node(AccelerometerParallaxNode::create());
            Functions::setNodeProperties(node, data);
            node->setMultiplier(Functions::pointForObjectKey(data, "multiplier",node->multiplier()));
            node->setFilteringFactor(Functions::floatForObjectKey(data, "filteringFactor",node->filteringFactor()));
            node->setAccelerationOffset(Functions::pointForObjectKey(data, "accelerationOffset",node->accelerationOffset()));
            return node;
            
        };
        
        _generators["label-ttf"]=[=](CCDictionary * data)->cocos2d::CCNode *{
            std::string text(Functions::stringForObjectKey(data, "text"));
            std::string fontName(Functions::stringForObjectKey(data, "font_name", "Georgia"));
            float fontSize(Functions::floatForObjectKey(data, "font_size", 24));
            
            ccColor3B color(Functions::colorForObjectKey(data, "text_color"));
            color=Functions::webColorForObjectKey(data, "text_webColor", color);
            CCTextAlignment hAlignment(Functions::textAlignmentForObjectKey(data, "alignment"));
            
            CCLabelTTF * letterLabel(CCLabelTTF::create(text.c_str(), fontName.c_str(), fontSize, CCSizeZero, hAlignment));
            Functions::setNodeProperties(letterLabel, data);
            letterLabel->setColor(color);
            return letterLabel;
        };
        
        _generators["label-bmf"]=[=](CCDictionary * data)->cocos2d::CCNode *{
            std::string text(Functions::stringForObjectKey(data, "text"));
            std::string fontPath(mcbPath(Functions::stringForObjectKey(data, "font_path")));
            assert(fontPath.length());
            
            CCTextAlignment hAlignment(Functions::textAlignmentForObjectKey(data, "alignment"));
            float width(Functions::floatForObjectKey(data, "width"));
            CCPoint imageOffset(Functions::pointForObjectKey(data, "imageOffset"));
            
            CCLabelBMFont * letterLabel(CCLabelBMFont::create(text.c_str(), fontPath.c_str(), width, hAlignment, imageOffset));
            Functions::setNodeProperties(letterLabel, data);
            if (data->objectForKey("text_color"))
                letterLabel->setColor(Functions::colorForObjectKey(data, "text_color"));
            else if(data->objectForKey("text_webColor"))
                letterLabel->setColor(Functions::webColorForObjectKey(data, "text_webColor"));
            
            return letterLabel;
        };
        
        _generators["menu-item-text"]=[=](CCDictionary * data)->cocos2d::CCNode *{
            std::string text(Functions::stringForObjectKey(data, "text"));
            std::string fontName(Functions::stringForObjectKey(data, "font_name", "Georgia"));
            float fontSize(Functions::floatForObjectKey(data, "font_size", 24));
            ccColor3B color(Functions::colorForObjectKey(data, "text_color"));
            color=Functions::webColorForObjectKey(data, "text_webColor",color);
            //CCTextAlignment hAlignment(textAlignmentForObjectKeyL(data, "alignment"));
            
            CCMenuItemFont * item=CCMenuItemFont::create(text.c_str());
            item->setTarget(_buttonEventReceiver, menu_selector(ViewBuilder::ButtonEventReceiver::_buttonWithTagPressed));
            Number * tag=(Number *)data->objectForKey("tag");
            if (tag)
                item->setTag(*tag);
            item->setColor(color);
            item->setFontName(fontName.c_str());
            item->setFontSize(fontSize);
            
            CCMenu*m(CCMenu::createWithItem(item));
            Functions::setNodeProperties(m, data);
            return m;
            
            
        };
        
        _generators["sprite"]=[=](CCDictionary * data)->CCNode *{
            std::string imagePath(Functions::stringForObjectKey(data, "image"));
            assert(imagePath.length());
            imagePath=mcbPath(imagePath);
            cocos2d::CCSprite * spr(cocos2d::CCSprite::create(imagePath.c_str()));
            Functions::setNodeProperties(spr, data);
            return spr;
            
        };
        
        _generators["scale9sprite"]=[=](CCDictionary * data)->CCNode *{
            std::string imagePath(Functions::stringForObjectKey(data, "image"));
            assert(imagePath.length());
            imagePath=mcbPath(imagePath);
            cocos2d::extension::CCScale9Sprite * spr(cocos2d::extension::CCScale9Sprite::create(imagePath.c_str()));
            Functions::setNodeProperties(spr, data);
            CCSize preferredSize(Functions::sizeForObjectKey(data, "preferredSize", spr->getContentSize()));
            spr->setPreferredSize(preferredSize);
            return spr;
        };
        
        _generators["menu-item-image"]=[=](CCDictionary * data)->CCNode *{
            CCAssert(data, "MenuItem data is missing!");
            
            std::string str(mcbPath(Functions::stringForObjectKey(data, "normalImage")));
            std::string str1(mcbPath(Functions::stringForObjectKey(data, "selectedImage")));
            
            assert(str.length());
            assert(str1.length());
            
            CCMenuItemImage * item(CCMenuItemImage::create(str.c_str(), str1.c_str()));
            item->setTarget(_buttonEventReceiver, menu_selector(ViewBuilder::ButtonEventReceiver::_buttonWithTagPressed));
            item->setTag(Functions::floatForObjectKey(data, "tag"));
            
            CCMenu*m(CCMenu::createWithItem(item));
            Functions::setNodeProperties(m, data);
            return m;
        };
        
        _generators["video-layer"]=[=](CCDictionary * data)->CCNode *{
            CCAssert(data, "Video Layer data is missing!");
            VideoLayer * retVal(VideoLayer::create());
            if (data->objectForKey("videoFrame"))
                retVal->setVideoFrame(Functions::rectForObjectKey(data, "videoFrame"));
            if (data->objectForKey("originalSizeToFill"))
                retVal->sizeToFill(Functions::sizeForObjectKey(data, "originalSizeToFill"));
            retVal->setVideoPath(mcbPath(Functions::stringForObjectKey(data, "videoPath")));
            Functions::setNodeProperties(retVal, data);
            return retVal;
        };
    }
    void ViewBuilder::setFactoryForKey(const std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)> & lambda, const std::string & key){
        _generators[key]=lambda;
    }
    std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)> ViewBuilder::factoryForKey(const std::string & key){
        auto it(_generators.find(key));
        if (it!=_generators.end())
            return it->second;
        return nullptr;
    }
    void ViewBuilder::buildViewWithSceneData(cocos2d::CCDictionary * sceneData){
        std::string viewDataStr("viewData");
        std::string deviceSufixViewDataStr(viewDataStr+PlatformSupport::getDeviceSuffix());
        std::string deviceIdiomViewDataStr(viewDataStr+PlatformSupport::getDeviceIdiom());
        
        CCDictionary * viewData((CCDictionary *)sceneData->objectForKey(deviceIdiomViewDataStr.c_str()));
        if (!viewData)
            viewData=((CCDictionary *)sceneData->objectForKey(deviceSufixViewDataStr.c_str()));
        if (!viewData)
            viewData=((CCDictionary *)sceneData->objectForKey(viewDataStr.c_str()));
        if (viewData)
            buildViewWithData(viewData);
    }
    void ViewBuilder::buildViewWithData(cocos2d::CCDictionary * data){
        if (!data)
            return;
        CCArray * children=(CCArray *)data->objectForKey("children");
        if (children)
            _populateChildren(children, _thisNode);
    }
    void ViewBuilder::_populateChildren(cocos2d::CCArray * children, CCNode * parent){
        
        mcbForEachBegin(CCDictionary *, child, children)
        CCString * typeS((CCString *)child->objectForKey("type"));
        if (typeS) {
            std::string type(typeS->m_sString);
            CCNode * childNode(nullptr);
            auto lambda(factoryForKey(type));
            if (lambda)
                childNode=lambda(child);
            
            if (childNode) {
                
                //binding dictionaries to nodes
                _originalDictionaryBinder.setOriginalDictionaryToDerivativeNodeNode(child, childNode);
                
                Number * tag=(Number *)child->objectForKey("tag");
                if (tag)
                    childNode->setTag(*tag);
                
                Number * zOrder=(Number *)child->objectForKey("zOrder");
                
                if (zOrder)
                    parent->addChild(childNode, *zOrder);
                else
                    parent->addChild(childNode);
                
                
                CCArray * m_children=(CCArray *)child->objectForKey("children");
                if (m_children){
                    if (dynamic_cast<CCMenu *>(childNode)!=nullptr)
                        _populateChildren(m_children, (CCNode *)(childNode->getChildren()->objectAtIndex(0)));
                    else
                        _populateChildren(m_children, childNode);
                }
            }
            
        }
        mcbForEachEnd
    }
#pragma mark callbacks
    ViewBuilder::ButtonEventReceiver::ButtonEventReceiver(callback_t callback): buttonHandler(callback){
    
    }
    ViewBuilder::ButtonEventReceiver::~ButtonEventReceiver(){
        
    }
    void ViewBuilder::ButtonEventReceiver::_buttonWithTagPressed(cocos2d::CCObject * button){
        CCMenuItem * item(dynamic_cast<CCMenuItem *>(button));
        if (!item || !buttonHandler)
            return;
        buttonHandler(item, item->getTag());
    }
    
#pragma mark Original Data binders
    cocos2d::CCDictionary * ViewBuilder::originalDataForChildNode(cocos2d::CCNode * node) const{
        return _originalDictionaryBinder.originalDictionaryForNode(node);
    }
    bool ViewBuilder::removeChildNodeWithOriginalData(cocos2d::CCNode * node, bool removeFromParent, bool cleanup){
        return _originalDictionaryBinder.removeChildNodeWithOriginalData(node, removeFromParent, cleanup);
    }
    std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> ViewBuilder::allChildrenWithOriginalData() const{
        return _originalDictionaryBinder.allChildrenWithOriginalData();
    }
    std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> ViewBuilder::childrenWithOriginalDataForTag(const int tag) const{
        return _originalDictionaryBinder.childrenWithOriginalDataForTag(tag);
    }
    std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> ViewBuilder::OriginalDictionaryBinder::allChildrenWithOriginalData() const{
        std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> retVal;
        retVal.reserve(_binders.size());
        for (const auto &p : _binders)
            retVal.emplace_back(std::make_pair(p.first, p.second->data()));
        return retVal;
    }
    std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> ViewBuilder::OriginalDictionaryBinder::childrenWithOriginalDataForTag(const int tag) const{
        std::vector<std::pair<cocos2d::CCNode *, cocos2d::CCDictionary *>> retVal;
        for (const auto &p : _binders)
            if (p.first->getTag()==tag)
                retVal.emplace_back(std::make_pair(p.first, p.second->data()));
        return retVal;
    }
    ViewBuilder::OriginalDictionaryBinder::NodeToDictionaryBinding::NodeToDictionaryBinding(cocos2d::CCDictionary * originalData, cocos2d::CCNode * node){
        _originalData=originalData;CC_SAFE_RETAIN(_originalData);_node=node;
    }
    ViewBuilder::OriginalDictionaryBinder::NodeToDictionaryBinding::~NodeToDictionaryBinding(){
        CC_SAFE_RELEASE(_originalData);
    }
    cocos2d::CCNode * ViewBuilder::OriginalDictionaryBinder::NodeToDictionaryBinding::node() const{
        return _node;
    }
    cocos2d::CCDictionary * ViewBuilder::OriginalDictionaryBinder::NodeToDictionaryBinding::data() const{
        return _originalData;
    }
    void ViewBuilder::OriginalDictionaryBinder::setOriginalDictionaryToDerivativeNodeNode(cocos2d::CCDictionary * originalData, cocos2d::CCNode * node){
        _binders[node]=std::make_shared<NodeToDictionaryBinding>(originalData, node);
        
    }
    cocos2d::CCDictionary * ViewBuilder::OriginalDictionaryBinder::originalDictionaryForNode(cocos2d::CCNode * node) const{
            auto it(_binders.find(node));
            if (it!=_binders.end())
                return it->second->data();
            return nullptr;
    }
    bool ViewBuilder::OriginalDictionaryBinder::removeChildNodeWithOriginalData(cocos2d::CCNode * node, bool removeFromParent, bool cleanup){
        auto it(_binders.find(node));
        if (it!=_binders.end()){
            _binders.erase(it);
            if (removeFromParent)
                node->removeFromParentAndCleanup(cleanup);
            return true;
        }
        return false;
    }
}}
