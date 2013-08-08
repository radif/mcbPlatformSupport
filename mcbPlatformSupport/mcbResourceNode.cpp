//
//  mcbResourceNode.cpp
//  mcb
//
//  Created by Radif Sharafullin on 6/9/13.
//
//

#include "mcbResourceNode.h"

#include "mcbPlatformSupport.h"
#include "mcbNumber.h"
#include "mcbPlatformSupportFunctions.h"
#include <stack>

using namespace cocos2d;
namespace mcb{namespace PlatformSupport{
    REGISTER_RESOURCE_NODE_CLASS_FOR_REFLECTION_CPP(ResourceNode);

    bool _isNavigationStackMarker(const std::string & markerCandidate){
        if (markerCandidate.empty())
            return false;
        return markerCandidate.front()==':';
    }
    static std::stack<std::string> _navigationStack;

#pragma mark lifecycle
    ResourceNode::ResourceNode(){
        
        
        auto setNodePropertiesL([](cocos2d::CCNode * node, CCDictionary * data){
        
            CCPoint pos(Functions::pointForObjectKey(data, "position"));
            float rotation(Functions::floatForObjectKey(data, "rotation"));
            
            //scale
            bool useScaleXY(false);
            float scaleX(1.f);
            float scaleY(1.f);
            
            float scale(1.f);
            Number * num((Number *)data->objectForKey("scale"));
            if (num)
                scale=*num;
            else{
                Number * pScaleX((Number *)data->objectForKey("scaleX"));
                Number * pScaleY((Number *)data->objectForKey("scaleY"));
                
                if (pScaleX || pScaleY) {
                    useScaleXY=true;
                    if (pScaleX)
                        scaleX=*pScaleX;
                    if (pScaleY)
                        scaleY=*pScaleY;
                }
                
            }
            
            if (data->objectForKey("anchorPoint"))
                node->setAnchorPoint(Functions::pointForObjectKey(data, "anchorPoint", {.5f,.5f}));
            
            if (data->objectForKey("contentSize"))
                node->setContentSize(Functions::sizeForObjectKey(data, "contentSize", {0.f,0.f}));

            node->setPosition(pos);
            
            if (useScaleXY){
                node->setScaleX(scaleX);
                node->setScaleY(scaleY);
            }else
                node->setScale(scale);
            
            node->setRotation(rotation);
        });
        
        _generators["node"]=[=](CCDictionary * data)->CCNode *{
            cocos2d::CCNode * node(cocos2d::CCNode::create());
            setNodePropertiesL(node, data);
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
            setNodePropertiesL(letterLabel, data);
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
            setNodePropertiesL(letterLabel, data);
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
            item->setTarget(this, menu_selector(ResourceNode::_buttonWithTagPressed));
            Number * tag=(Number *)data->objectForKey("tag");
            if (tag)
                item->setTag(*tag);
            item->setColor(color);
            item->setFontName(fontName.c_str());
            item->setFontSize(fontSize);
            
            CCMenu*m(CCMenu::createWithItem(item));
            setNodePropertiesL(m, data);
            return m;
            
            
        };
        
        _generators["sprite"]=[=](CCDictionary * data)->CCNode *{
            std::string imagePath(Functions::stringForObjectKey(data, "image"));
            assert(imagePath.length());
            imagePath=mcbPath(imagePath);
            cocos2d::CCSprite * spr(cocos2d::CCSprite::create(imagePath.c_str()));
            setNodePropertiesL(spr, data);
            return spr;
            
        };
        
        _generators["menu-item-image"]=[=](CCDictionary * data)->CCNode *{
            CCAssert(data, "MenuItem data is missing!");
            
            std::string str(mcbPath(Functions::stringForObjectKey(data, "normalImage")));
            std::string str1(mcbPath(Functions::stringForObjectKey(data, "selectedImage")));

            assert(str.length());
            assert(str1.length());
            
            CCMenuItemImage * item=CCMenuItemImage::create(str.c_str(), str1.c_str());
            item->setTarget(this, menu_selector(ResourceNode::_buttonWithTagPressed));
            item->setTag(Functions::floatForObjectKey(data, "tag"));
            
            CCMenu*m(CCMenu::createWithItem(item));
            setNodePropertiesL(m, data);
            return m;
        };
        
    }
    ResourceNode::~ResourceNode(){
        CC_SAFE_RELEASE(_data);
    };

    bool ResourceNode::init(cocos2d::CCDictionary * data){
        auto retVal(super::init());
        std::string viewDataStr("viewData");
        std::string deviceSufixViewDataStr(viewDataStr+PlatformSupport::getDeviceSuffix());
        std::string deviceIdiomViewDataStr(viewDataStr+PlatformSupport::getDeviceIdiom());
        
        CCDictionary * viewData((CCDictionary *)data->objectForKey(deviceIdiomViewDataStr.c_str()));
        if (!viewData)
            viewData=((CCDictionary *)data->objectForKey(deviceSufixViewDataStr.c_str()));
        if (!viewData)
            viewData=((CCDictionary *)data->objectForKey(viewDataStr.c_str()));
        if (viewData)
            buildViewWithData(viewData);
        
        return retVal;
    }
    void ResourceNode::setData(cocos2d::CCDictionary * data){
        CC_SAFE_RELEASE(_data);
        _data=data;
        CC_SAFE_RETAIN(_data);
    }

#pragma mark construction
    
    cocos2d::CCScene * ResourceNode::createWithScene(const std::string & localPath){
        CCScene *scene(CCScene::create());
        while (!_navigationStack.empty())
            _navigationStack.pop();
        _navigationStack.push(localPath);
        auto node(mcb::PlatformSupport::ResourceNode::create(localPath));
        scene->addChild(node);
        return scene;
    }
    
    ResourceNode * ResourceNode::create(const std::string & localPath){
        const std::string path(PlatformSupport::resolvePath(localPath));
        CCDictionary * data(PlatformSupport::dictionaryFromPlist(std::string(path+"/manifest.data").c_str()));
        if (data)
            return create(path, data);
        return nullptr;
    }
    ResourceNode * ResourceNode::create(const std::string & localPath, cocos2d::CCDictionary * data){
        ResourceNode * retVal(nullptr);
        CCString * name((CCString *)data->objectForKey("class"));
        if (name) {
            retVal=ResourceNodeFactory::createInstance(name->m_sString);
            if (retVal) {
                retVal->_localPath=localPath;
                retVal->setData(data);
                if (!retVal->init(data)){
                    delete retVal;
                    retVal = nullptr;
                }
                    
            }
        }
        
        return retVal;
    }
#pragma mark transition
    void ResourceNode::mcbCleanup(){
        ;//when override, please, call super
    }
    void ResourceNode::_processCleanup(){
        if (_mcbCleanupCalled)
            return;
        beforeCleanup();
        mcbCleanup();
        _mcbCleanupCalled=true;
        afterCleanup();
    }
    
    void ResourceNode::beforeCleanup(){
        ;//when override, please, call super
    }
    void ResourceNode::afterCleanup(){
        ;//when override, please, call super
    }
    
    
    bool ResourceNode::transitionBack(const std::function<void(ResourceNode * newNode, const std::function<void()> & transitionHandle)> & transitionHandle, const std::string & marker){
        if (_navigationStack.empty())
            return false;
        
        if (!marker.empty())
            popNavigationStackUntilMarker(marker);
        
        
        _navigationStack.pop();
        std::string prevPath;
        if (!_navigationStack.empty()) {
            prevPath=_navigationStack.top();
            
            while (_isNavigationStackMarker(prevPath) && !_navigationStack.empty()) {
                _navigationStack.pop();
                prevPath=_navigationStack.top();
            }
            
            return transitionToANodeWithPath(prevPath, transitionHandle, false);
        }
        return false;
    }
    bool ResourceNode::transitionToANodeWithPath(const std::string & path, const std::function<void(ResourceNode * newNode, const std::function<void()> & transitionHandle)> & transitionHandle, bool isStack){
        std::string fullPath(mcbPath(path));
        if (isStack)
            _navigationStack.push(fullPath);
        
        ResourceNode * newNode(create(fullPath));
        if (!newNode)
            return false;
        
        auto completedHandle([=](){
            _processCleanup();
            auto p(getParent());
            p->removeChild(this);
            p->addChild(newNode);
        });
        
        if (transitionHandle) {
            newNode->retain();
            transitionHandle(newNode, [=]{
                completedHandle();
                newNode->release();
            });
        }else
            completedHandle();
        
        return true;
    }
    
    void ResourceNode::popNavigationStack(){
        if (!_navigationStack.empty())
            _navigationStack.pop();
    }
    void ResourceNode::rewindNavigationStack(){
        while (!_navigationStack.empty())
            _navigationStack.pop();
    }
    void ResourceNode::pushNavigationStackMarker(const std::string & marker){
        _navigationStack.push(':'+marker);
    }
    void ResourceNode::popNavigationStackUntilMarker(const std::string & marker){
        std::string m(':'+marker);
        while (!_navigationStack.empty()){
            if (_navigationStack.top()!=m)
                _navigationStack.pop();
            else
                break;
        }
    }
#pragma mark view builder
    void ResourceNode::setFactoryForKey(const std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)> & lambda, const std::string & key){
        _generators[key]=lambda;
    }
    std::function<cocos2d::CCNode *(cocos2d::CCDictionary *)> ResourceNode::factoryForKey(const std::string & key){
        auto it(_generators.find(key));
        if (it!=_generators.end())
            return (*it).second;
        return nullptr;
    }
    void ResourceNode::buildViewWithData(cocos2d::CCDictionary * data){
        if (!data)
            return;
        CCArray * children=(CCArray *)data->objectForKey("children");
        _populateChildren(children, this);
    }
    void ResourceNode::_populateChildren(cocos2d::CCArray * children, CCNode * parent){
        
        mcbForEachBegin(CCDictionary *, child, children)
        CCString * typeS((CCString *)child->objectForKey("type"));
        if (typeS) {
            std::string type(typeS->m_sString);
            CCNode * childNode(nullptr);
            auto lambda(factoryForKey(type));
            if (lambda)
                childNode=lambda(child);
            
            if (childNode) {
                
                Number * tag=(Number *)child->objectForKey("tag");
                if (tag)
                    childNode->setTag(*tag);
                
                Number * zOrder=(Number *)child->objectForKey("zOrder");
                
                if (zOrder)
                    parent->addChild(childNode, *zOrder);
                else
                    parent->addChild(childNode);
                
                
                CCArray * m_children=(CCArray *)child->objectForKey("children");
                if (m_children)
                    _populateChildren(m_children, childNode);
            }
            
        }
        mcbForEachEnd
    }
#pragma mark callbacks
    void ResourceNode::_buttonWithTagPressed(cocos2d::CCObject * button){
        CCMenuItem * item(dynamic_cast<CCMenuItem *>(button));
        if (!item)
            return;
        buttonWithTagPressed(button, item->getTag());
    }
}}