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
    ResourceNode::ResourceNode(): ViewBuilder(this){
    
    }
    ResourceNode::~ResourceNode(){
        CC_SAFE_RELEASE(_data);
    };

    bool ResourceNode::init(cocos2d::CCDictionary * data){
        auto retVal(super::init());
        buildViewWithSceneData(data);
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
        CCDictionary * data(PlatformSupport::dictionaryFromPlist(Functions::stringByAppendingPathComponent(path, Constants::kManifestDataFile)));
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
    bool ResourceNode::reloadNode(const std::function<void(ResourceNode * newNode, const std::function<void()> & transitionHandle)> & transitionHandle){
        return transitionToANodeWithPath(_localPath, transitionHandle, false);
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
}}