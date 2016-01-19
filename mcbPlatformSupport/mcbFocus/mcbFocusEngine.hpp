//
//  mcbFocusEngine.hpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/19/16.
//
//

#ifndef mcbFocusEngine_hpp
#define mcbFocusEngine_hpp

#include "mcbFactory.h"
namespace mcb{namespace PlatformSupport{
    class FocusEngine : public SingletonFactory<FocusEngine>{
    public:
        virtual void init() override;
        virtual ~FocusEngine()=default;
        
        void addFocusableNode(cocos2d::CCNode * node, const std::function<void(cocos2d::CCNode * node)> action=nullptr);
        
        void removeFocusableNode(cocos2d::CCNode * node);
        
        void removeAllFocusableNodes();
    };
}}
#endif /* mcbFocusEngine_hpp */
