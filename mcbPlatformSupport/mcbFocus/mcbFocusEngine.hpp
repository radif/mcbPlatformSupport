//
//  mcbFocusEngine.hpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/19/16.
//
//

#ifndef mcbFocusEngine_hpp
#define mcbFocusEngine_hpp

#include "mcbPressable.hpp"

namespace mcb{namespace PlatformSupport{
    class FocusEngine{
        class MCBTouchablePressable: public cocos2d::CCObject, public cocos2d::CCTargetedTouchDelegate, public Pressable{
            FocusEngine * _focusEngine=nullptr;
        public:
            MCBTouchablePressable(FocusEngine * focusEngine, int priority, bool swallowsInput);
            virtual ~MCBTouchablePressable();
        protected:
            //presses for the remote
            virtual void pressBegan(const mcb::PlatformSupport::p_Press & press) override;
            virtual void pressChanged(const mcb::PlatformSupport::p_Press & press) override;
            virtual void pressEnded(const mcb::PlatformSupport::p_Press & press) override;
            virtual void pressCancelled(const mcb::PlatformSupport::p_Press & press) override;
            
            
            //touches for the focus engine
            virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
            // optional
            virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
            virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
            virtual void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
        };
        friend class MCBTouchablePressable;
        typedef std::shared_ptr<MCBTouchablePressable> pMCBTouchablePressable;
        pMCBTouchablePressable _mcbTouchablePressable=nullptr;
        bool _managedInputAnimated=true;
    protected:
        typedef enum FocusState{
            FocusStateUndefined=-1,
            FocusStateNormal=0,
            FocusStateFocused=1,
            FocusStatePressed=2,
        }FocusState;
        typedef std::function<void(cocos2d::CCNode * node, const FocusState focusState, bool animated)> focus_action_t;
        typedef std::function<void(cocos2d::CCNode * node)> trigger_action_t;
    private:
        struct Focusable{
            //ctor
            Focusable(cocos2d::CCNode * node, const focus_action_t & focusAction, const trigger_action_t & triggerAction, cocos2d::CCDictionary * focusContext);
            
            void focus(FocusState focusState, bool animated) const;
            const std::function<void()> getTriggerable() const;//cannot trigget directly since it can be distructive to this object, resulting in a crash
            
            //updateable positions
            cocos2d::CCNode * _upNode=nullptr;
            cocos2d::CCNode * _upLeftNode=nullptr;
            cocos2d::CCNode * _upRightNode=nullptr;
            cocos2d::CCNode * _downNode=nullptr;
            cocos2d::CCNode * _downLeftNode=nullptr;
            cocos2d::CCNode * _downRightNode=nullptr;
            cocos2d::CCNode * _leftNode=nullptr;
            cocos2d::CCNode * _rightNode=nullptr;
            
            //focus context
            struct FocusContext{
                std::string focusID;
                bool isFocused;
                std::string up, upLeft, upRight, down, downLeft, downRight, left, right;
            };
            typedef std::shared_ptr<FocusContext> pFocusContext;
            pFocusContext _focusContext=nullptr;
        private:
            
            const cocos2d::CCNode * _node;
            FocusState _focusState=FocusStateUndefined;
            const focus_action_t _focusAction;
            const trigger_action_t _triggerAction;
        };
        
        std::map<cocos2d::CCNode *, Focusable> _focusables;
        
        cocos2d::CCNode *_currentlyFocusedNode=nullptr;
        
        struct {
            cocos2d::CCPoint lastSelectionLocation;
            cocos2d::CCPoint currentLocation;
            float kJumpingSwipeDistance=300.0f;
            float kDiagonalDistanceMultiplier=.75f;
            float veritcalSwipeDistanceMultiplier=1.7f;
        } _swipeContext;
        void _updateSelection();
        
        cocos2d::CCNode * _moveFocusToNode(const std::function<cocos2d::CCNode * (Focusable * currentlyFocusedNode)> & selectorHandle);
        
    protected:
        FocusEngine()=default;
        virtual ~FocusEngine()=default;
        
        //managing focusables
        virtual void addFocusableNode(cocos2d::CCNode * node, const focus_action_t & focusAction=nullptr, const trigger_action_t & triggerAction=nullptr, cocos2d::CCDictionary * focusContext=nullptr);
        
        virtual void removeFocusableNode(cocos2d::CCNode * node);//custom manupulation
        
        virtual void removeAllFocusableNodes();//use this when the current scene exits
        
        //sorting
        virtual void sortFocusableNodesByFocusContext();//all nodes must have parents
        virtual void sortFocusableNodesAsInOrder(const std::vector<cocos2d::CCNode *> & nodesInOrder, bool isHorizontally=true);//all nodes must have parents
        
        //deprecated sorting - must be improved or removed!
        virtual void sortFocusableNodesByCurrentPositions(bool isHorizontalLayout=true);//all nodes must have parents
        

        //manual remote control mode
        //swipe touch methods
        virtual void setJumpingSwipeDistance(const float jumpingSwipeDistance){_swipeContext.kJumpingSwipeDistance=jumpingSwipeDistance;}
        virtual const float jumpingSwipeDistance() const{return _swipeContext.kJumpingSwipeDistance;}
        
        virtual void setVeritcalSwipeDistanceMultiplier(const float veritcalSwipeDistanceMultiplier){_swipeContext.veritcalSwipeDistanceMultiplier=veritcalSwipeDistanceMultiplier;}
        virtual const float veritcalSwipeDistanceMultiplier() const{return _swipeContext.veritcalSwipeDistanceMultiplier;}
        
        virtual void setDiagonalDistanceMultiplier(const float diagonalDistanceMultiplier){_swipeContext.kDiagonalDistanceMultiplier=diagonalDistanceMultiplier;}
        virtual const float diagonalDistanceMultiplier() const{return _swipeContext.kDiagonalDistanceMultiplier;}


        virtual cocos2d::CCNode * focusSwipeBegan(const cocos2d::CCPoint & worldLocation);
        virtual cocos2d::CCNode * focusSwipeMoved(const cocos2d::CCPoint & worldLocation);
        virtual cocos2d::CCNode * focusSwipeEnded(const cocos2d::CCPoint & worldLocation);
        
        //press methods
        virtual cocos2d::CCNode * processDirectionalPress(const Press::Type & type, const bool animated=true);
        
        //focus shift
        virtual cocos2d::CCNode * moveFocusUp(bool animated=true);
        virtual cocos2d::CCNode * moveFocusUpLeft(bool animated=true);
        virtual cocos2d::CCNode * moveFocusUpRight(bool animated=true);
        virtual cocos2d::CCNode * moveFocusDown(bool animated=true);
        virtual cocos2d::CCNode * moveFocusDownLeft(bool animated=true);
        virtual cocos2d::CCNode * moveFocusDownRight(bool animated=true);
        virtual cocos2d::CCNode * moveFocusRight(bool animated=true);
        virtual cocos2d::CCNode * moveFocusLeft(bool animated=true);
        
        virtual cocos2d::CCNode * currentlyFocusedNode(){return _currentlyFocusedNode;}
        
        virtual std::string focusIDForNode(cocos2d::CCNode * node);//available if has focus context, otherwise returns an empty string
        virtual cocos2d::CCNode * nodeWithFocusID(const std::string & focusID);
        
        virtual void setCurrentlyFocusedNode(cocos2d::CCNode * node, bool withFocusAction, bool animated);

        virtual cocos2d::CCNode * setCurrentlyFocusedNodePressed(bool pressed, bool animated=true);
        
        virtual cocos2d::CCNode * triggerCurrentlyFocusedNode();
        
        virtual void triggerSelectionForFocusableNode(cocos2d::CCNode * node);
        
        //managed remote control mode
        virtual void beginManagedInput(int priority=100, bool swallowsInput=true);
        virtual void endManagedInput();
        
        void setManagedInputAnimated(bool animated){_managedInputAnimated=animated;}
        bool isManagedInputAnimated() const{return _managedInputAnimated;}
        
        //remote control event receivers and overrides (you need to call super once override for managed input to work)
        
        virtual void mcbPressBegan(const p_Press & press);
        virtual void mcbPressChanged(const p_Press & press);
        virtual void mcbPressEnded(const p_Press & press);
        virtual void mcbPressCancelled(const p_Press & press);
        
        //convenience: don't need to call super
        virtual void mcbRemoteButtonPressed(const p_Press & press){}
        
        virtual bool mcbTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
        virtual void mcbTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
        virtual void mcbTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
        virtual void mcbTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    };
}}
#endif /* mcbFocusEngine_hpp */
