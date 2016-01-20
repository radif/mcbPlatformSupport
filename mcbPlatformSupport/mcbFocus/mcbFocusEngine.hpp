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
        
    public:
        virtual void init() override;
        virtual ~FocusEngine()=default;
        
        void addFocusableNode(cocos2d::CCNode * node, const std::function<void(cocos2d::CCNode * node, const bool isFocused, bool animated)> & focusAction=nullptr, const std::function<void(cocos2d::CCNode * node)> & triggerAction=nullptr);
        
        void removeFocusableNode(cocos2d::CCNode * node);//custom manupulation
        
        void removeAllFocusableNodes();//use this when the current scene exits
        
        void sortByCurrentPositions();//all nodes must have parents
        
        
        //swipe touch methods
        cocos2d::CCNode * swipeBegan(const cocos2d::CCPoint & worldLocation);
        cocos2d::CCNode * swipeMoved(const cocos2d::CCPoint & worldLocation);
        cocos2d::CCNode * swipeEnded(const cocos2d::CCPoint & worldLocation);
        
        //focus shift
        cocos2d::CCNode * moveSelectionRight(bool animated=true);
        cocos2d::CCNode * moveSelectionLeft(bool animated=true);
        cocos2d::CCNode * moveSelectionUp(bool animated=true);
        cocos2d::CCNode * moveSelectionDown(bool animated=true);
        
        cocos2d::CCNode * currentlySelectedNode(){return _currentlySelectedNode;}
        void setCurrentlySelectedNode(cocos2d::CCNode * node, bool withFocusAction, bool animated);
        
        cocos2d::CCNode * triggerCurrentSelection();
        
        void triggerSelectionForNode(cocos2d::CCNode * node);
    };
}}
#endif /* mcbFocusEngine_hpp */
