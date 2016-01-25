//
//  mcbPressable.hpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/14/16.
//
//

#ifndef mcbPressable_hpp
#define mcbPressable_hpp

#include "mcbPress.hpp"
namespace mcb{namespace PlatformSupport{
    class Pressable{
        friend class PressDispatcher;
        int __priority=0;
        bool __swallowsPresses=false;
    public:
        virtual void pressBegan(const p_Press & press){}
        virtual void pressChanged(const p_Press & press){}
        virtual void pressEnded(const p_Press & press){}
        virtual void pressCancelled(const p_Press & press){}
    protected:
        virtual void startListeningPresses(const int priority=0, const bool swallowsPresses=false);
        virtual void stopListeningPresses();
        virtual ~Pressable();
    };
}}

#endif /* mcbPressable_hpp */
