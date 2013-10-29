//
//  mcbMainThreadCaller.cpp
//  mcb
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#include "mcbMainThreadCaller.h"
#include "mcbFactory.h"
#include <future>

namespace mcb{namespace PlatformSupport{
    class MainThreadCaller;
    class MainThreadCaller : public cocos2d::CCObject, public PlatformSupport::SingletonFactory<MainThreadCaller> {
        friend PlatformSupport::SingletonFactory<MainThreadCaller>;
        virtual void init() override{}
        std::vector<std::function<void()>> _functions;
        std::mutex _functionsMutex;
        
        void flush(){
            decltype(_functions) functionsCopy;
            {
                std::lock_guard<std::mutex> lock(_functionsMutex);
                functionsCopy=std::move(_functions);
                using cocos2d::SEL_SCHEDULE;
                cocos2d::CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(MainThreadCaller::flush), this);
                _functions.erase(_functions.begin(), _functions.end());
                assert(_functions.size()==0);
            }
            //call the functions outside the mutext lock
            //functionsCopy is always on main thread, so no data races
            for (auto i=functionsCopy.begin(); i!=functionsCopy.end(); ++i)
                (*i)();
        }
        
    public:
        void call_on_main_thread(std::function<void()> && mainThreadFunction, bool waitUntilDone=false){
            {
            std::lock_guard<std::mutex> lock(_functionsMutex);
            _functions.push_back(std::move(mainThreadFunction));
            using cocos2d::SEL_SCHEDULE;
            cocos2d::CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(MainThreadCaller::flush), this);
            cocos2d::CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(MainThreadCaller::flush), this, 0, false);
            }
            
            if (waitUntilDone)
                std::this_thread::sleep_for(std::chrono::microseconds(50));// or use this?: std::this_thread::yield();
                //std::this_thread::yield();
        }
        
        
    };
    
    
    void call_on_main_thread(std::function<void()> mainThreadFunction, bool waitUntilDone){
        MainThreadCaller::sharedInstance()->call_on_main_thread(std::move(mainThreadFunction), waitUntilDone);
    }
}}

