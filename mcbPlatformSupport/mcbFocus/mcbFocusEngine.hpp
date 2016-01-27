//
//  mcbFocusEngine.hpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/19/16.
//
//

#ifndef mcbFocusEngine_hpp
#define mcbFocusEngine_hpp

#include "cocos2d.h"
namespace mcb{namespace PlatformSupport{
    class FocusEngine{
        struct Focusable{
            //ctor
            Focusable(cocos2d::CCNode * node, const std::function<void(cocos2d::CCNode * node, const bool isFocused, bool animated)> & focusAction, const std::function<void(cocos2d::CCNode * node)> & triggerAction);
            
            void focus(bool m_isFocused, bool animated) const;
            const std::function<void()> getTriggerable() const;//cannot trigget directly since it can be distructive to this object, resulting in a crash
            
            //updateable positions
            cocos2d::CCNode * _upNode=nullptr;
            cocos2d::CCNode * _downNode=nullptr;
            cocos2d::CCNode * _leftNode=nullptr;
            cocos2d::CCNode * _rightNode=nullptr;
        private:
            const cocos2d::CCNode * _node;
            bool _isFocused=false;
            const std::function<void(cocos2d::CCNode * node, const bool isFocused, bool animated)> _focusAction;
            const std::function<void(cocos2d::CCNode * node)> _triggerAction;
        };
        
        std::map<cocos2d::CCNode *, Focusable> _focusables;
        
        cocos2d::CCNode *_currentlySelectedNode=nullptr;
        
        struct {
            cocos2d::CCPoint lastSelectionLocation;
            cocos2d::CCPoint currentLocation;
            const float kJumpingSwipeDistance=120.0f;
        } _swipeContext;
        void _updateSelection();
        
    protected:
        FocusEngine()=default;
        virtual ~FocusEngine()=default;
        
        void addFocusableNode(cocos2d::CCNode * node, const std::function<void(cocos2d::CCNode * node, const bool isFocused, bool animated)> & focusAction=nullptr, const std::function<void(cocos2d::CCNode * node)> & triggerAction=nullptr);
        
        void removeFocusableNode(cocos2d::CCNode * node);//custom manupulation
        
        void removeAllFocusableNodes();//use this when the current scene exits
        
        void sortFocusableNodesByCurrentPositions();//all nodes must have parents
        
        
        //swipe touch methods
        cocos2d::CCNode * focusSwipeBegan(const cocos2d::CCPoint & worldLocation);
        cocos2d::CCNode * focusSwipeMoved(const cocos2d::CCPoint & worldLocation);
        cocos2d::CCNode * focusSwipeEnded(const cocos2d::CCPoint & worldLocation);
        
        //focus shift
        cocos2d::CCNode * moveFocusRight(bool animated=true);
        cocos2d::CCNode * moveFocusLeft(bool animated=true);
        cocos2d::CCNode * moveFocusUp(bool animated=true);
        cocos2d::CCNode * moveFocusDown(bool animated=true);
        
        cocos2d::CCNode * currentlyFocusedNode(){return _currentlySelectedNode;}
        void setCurrentlyFocusedNode(cocos2d::CCNode * node, bool withFocusAction, bool animated);
        
        cocos2d::CCNode * triggerCurrentlyFocusedNode();
        
        void triggerSelectionForFocusableNode(cocos2d::CCNode * node);
    };
}}
#endif /* mcbFocusEngine_hpp */
