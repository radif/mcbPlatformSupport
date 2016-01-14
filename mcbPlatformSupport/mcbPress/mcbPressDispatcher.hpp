//
//  mcbPressDispatcher.hpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/14/16.
//
//

#ifndef mcbPressDispatcher_hpp
#define mcbPressDispatcher_hpp

#include "mcbFactory.h"
#include "mcbPressable.hpp"

namespace mcb{namespace PlatformSupport{
    class PressDispatcher : public SingletonFactory<PressDispatcher>{
        bool _menuButtonClosesApp=true;
    public:
        void setMenuButtonClosesApp(const bool menuButtonClosesApp);
        bool menuButtonClosesApp();
        virtual void init() override;
        
        //dispatch
        void dispatchPressBegan(const p_Press & press);
        void dispatchPressChanged(const p_Press & press);
        void dispatchPressEnded(const p_Press & press);
        void dispatchPressCancelled(const p_Press & press);
        
    };
}}

#endif /* mcbPressDispatcher_hpp */
