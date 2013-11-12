//
//  mcbResourceNode.h
//  mcb
//
//  Created by Radif Sharafullin on 6/9/13.
//
//

#ifndef __mcb__mcbResourceNode__
#define __mcb__mcbResourceNode__

#include "cocos2d.h"
#include "mcbPath.h"
#include "mcbResourceNodeReflection.h"
#include "mcbViewBuilder.h"

namespace mcb{namespace PlatformSupport{
    class ResourceNode : public cocos2d::CCNode, public ViewBuilder{
        typedef cocos2d::CCNode super;
        
        cocos2d::CCDictionary * _data=nullptr;
        
        virtual void setData(cocos2d::CCDictionary * data);
        friend class ResourceNodeFactory;
        REGISTER_RESOURCE_NODE_CLASS_FOR_REFLECTION_H(ResourceNode);
        
        //private parts
        bool _mcbCleanupCalled=false;
        virtual void _processCleanup();
    public:
        
        static cocos2d::CCScene * createWithScene(const std::string & localPath);
        
        template <typename T>
        static T create(const std::string & localPath){return dynamic_cast<T>(ResourceNode::create(localPath));}
        static ResourceNode * create(const std::string & localPath);
        static ResourceNode * create(const std::string & localPath, cocos2d::CCDictionary * data);
        virtual bool init(cocos2d::CCDictionary * data);
        
        bool transitionToANodeWithPath(const std::string & path, const std::function<void(ResourceNode * newNode, const std::function<void()> & transitionHandle)> & transitionHandle=nullptr, bool isStack=true);
        bool transitionBack(const std::function<void(ResourceNode * newNode, const std::function<void()> & transitionHandle)> & transitionHandle=nullptr, const std::string & marker="");
        bool reloadNode(const std::function<void(ResourceNode * newNode, const std::function<void()> & transitionHandle)> & transitionHandle=nullptr);
        
        static void popNavigationStack();
        static void rewindNavigationStack();
        static void pushNavigationStackMarker(const std::string & marker);
        static void popNavigationStackUntilMarker(const std::string & marker);
        
    protected:
        virtual void beforeCleanup();//when override, please, call super
        virtual void afterCleanup();//when override, please, call super
        
        virtual void mcbCleanup(); //when override, please, call super
        virtual cocos2d::CCDictionary * getData(){return _data;}
        ResourceNode();
        virtual ~ResourceNode();
    };
}}
#endif /* defined(__mcb__mcbResourceNode__) */
