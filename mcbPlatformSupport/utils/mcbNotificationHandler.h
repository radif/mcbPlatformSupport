//
//  mcbNotificationHandler.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/23/14.
//
//

#ifndef __mcb__utils__mcbNotificationHandler__
#define __mcb__utils__mcbNotificationHandler__

#include "cocos2d.h"
namespace mcb{ namespace PlatformSupport{
    class NotificationHandler;
    typedef std::shared_ptr<NotificationHandler> pNotificationHandler;
    class NotificationHandler : public cocos2d::CCObject{
        std::map<std::string, std::pair<std::function<void()>, cocos2d::CCString *>> _handlers;
    public:
        static pNotificationHandler create(){return std::make_shared<NotificationHandler>();}
        void _notificationReceived(cocos2d::CCString * name){
            if (!name)
                return;
            auto it(_handlers.find(name->m_sString));
            if (it!=_handlers.end())
                it->second.first();
        }
        void addNotificationHandler(const std::string & name, const std::function<void()>& handler){
            using namespace cocos2d;
            removeNotificationHandler(name);
            CCString * nameS(CCString::create(name));
            nameS->retain();
            
            _handlers[name]={handler, nameS};
            
            CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(NotificationHandler::_notificationReceived), name.c_str(), nameS);
        }
        void removeNotificationHandler(const std::string & name){
            using namespace cocos2d;
            auto it(_handlers.find(name));
            if (it!=_handlers.end()) {
                CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, it->first.c_str());
                it->second.second->release();
                _handlers.erase(it);
            }
        }
        void removeAllNotificationHandlers(){
            auto handlersCopy(_handlers);
            for (const auto &p : handlersCopy)
                removeNotificationHandler(p.first);
        }
        virtual ~NotificationHandler(){
            removeAllNotificationHandlers();
        }
    };
    
    //base class for more convenience
    class Notification{
        pNotificationHandler _notificationHandler=nullptr;
    public:
        Notification(){_notificationHandler=NotificationHandler::create();}
        virtual ~Notification()=default;
    protected:
        virtual void addNotificationHandler(const std::string & name, const std::function<void()>& handler){
            _notificationHandler->addNotificationHandler(name, handler);
        }
        virtual void removeNotificationHandler(const std::string & name){
            _notificationHandler->removeNotificationHandler(name);
        }
        virtual void removeAllNotificationHandlers(){
            _notificationHandler->removeAllNotificationHandlers();
        }
    };

}}
#endif /* defined(__mcb__utils__mcbNotificationHandler__) */
