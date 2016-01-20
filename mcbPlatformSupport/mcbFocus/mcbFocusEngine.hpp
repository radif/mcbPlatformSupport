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
            const cocos2d::CCNode * node;
            const std::function<void(cocos2d::CCNode * node, const bool isFocused, bool animated)> focusAction;
            const std::function<void(cocos2d::CCNode * node)> triggerAction;
            
            //ctor
            Focusable(cocos2d::CCNode * node, const std::function<void(cocos2d::CCNode * node, const bool isFocused, bool animated)> & focusAction, const std::function<void(cocos2d::CCNode * node)> & triggerAction)
            : node(node)
            , focusAction(focusAction)
            , triggerAction(triggerAction)
            {}

            //focused property managed by the wrapper class
            bool isFocused=false;
            
            void focus(bool m_isFocused, bool animated) const{
                if(m_isFocused!=isFocused){
                    const_cast<Focusable *>(this)->isFocused=m_isFocused;
                    if(focusAction)
                        focusAction(const_cast<cocos2d::CCNode *>(node), isFocused, animated);
                }
            }
            void trigger() const{
            if(triggerAction)
                triggerAction(const_cast<cocos2d::CCNode *>(node));
            }
            //updateable positions
            cocos2d::CCNode * _upNode=nullptr;
            cocos2d::CCNode * _downNode=nullptr;
            cocos2d::CCNode * _leftNode=nullptr;
            cocos2d::CCNode * _rightNode=nullptr;
        };
        
        std::map<cocos2d::CCNode *, Focusable> _focusables;
        
        cocos2d::CCNode *_currentlySelectedNode=nullptr;
        
        struct {
            cocos2d::CCPoint lastSelectionLocation;
            cocos2d::CCPoint currentLocation;
            const float kJumpingSwipeDistance=60.0f;
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
        void swipeBegan(const cocos2d::CCPoint & worldLocation);
        void swipeMoved(const cocos2d::CCPoint & worldLocation);
        void swipeEnded(const cocos2d::CCPoint & worldLocation);
        
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
