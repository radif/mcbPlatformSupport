//
//  mcbFocusEngine.cpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/19/16.
//
//

#include "mcbFocusEngine.hpp"
#include "mcbPlatformSupportFunctions.h"

namespace mcb{namespace PlatformSupport{
    using namespace cocos2d;
    
    void FocusEngine::addFocusableNode(cocos2d::CCNode * node, const focus_action_t & focusAction, const trigger_action_t & triggerAction, cocos2d::CCDictionary * focusContext){
        removeFocusableNode(node);
        _focusables.insert({node, Focusable{node, focusAction, triggerAction, focusContext}});
    }
    
    void FocusEngine::removeFocusableNode(cocos2d::CCNode * node){
        auto it(_focusables.find(node));
        if (it!=_focusables.end())
            _focusables.erase(it);
    }
    
    void FocusEngine::removeAllFocusableNodes(){
        _currentlyFocusedNode=nullptr;
        _focusables.erase(_focusables.begin(), _focusables.end());
    }
    
    
    //soring by context
    void FocusEngine::sortFocusableNodesByFocusContext(bool animateInitialFocus){
        //create a map for faster access
        std::map<std::string, cocos2d::CCNode *> focusablesByFocusID;
        for (const auto & p : _focusables)
            if (p.second._focusContext)
                focusablesByFocusID[p.second._focusContext->focusID]=p.first;
        
        const auto nodeByFocusID([&](const std::string &focusID)->CCNode *{
            auto it(focusablesByFocusID.find(focusID));
            if (it!=focusablesByFocusID.end())
                return it->second;
            
            return nullptr;
        });
        
        
        CCNode * nodeToFocus(nullptr);
        
        for (auto & p : _focusables)
            if (p.second._focusContext){
                if (p.second._focusContext->isFocused)
                    nodeToFocus=p.first;
                
                //mapping
                const Focusable::pFocusContext & focusContext(p.second._focusContext);
                
                p.second._upNode=nodeByFocusID(focusContext->up);
                p.second._upLeftNode=nodeByFocusID(focusContext->upLeft);
                p.second._upRightNode=nodeByFocusID(focusContext->upRight);
                p.second._downNode=nodeByFocusID(focusContext->down);
                p.second._downLeftNode=nodeByFocusID(focusContext->downLeft);
                p.second._downRightNode=nodeByFocusID(focusContext->downRight);
                p.second._leftNode=nodeByFocusID(focusContext->left);
                p.second._rightNode=nodeByFocusID(focusContext->right);
            }
        
        
        if (nodeToFocus)
            setCurrentlyFocusedNode(nodeToFocus, true, animateInitialFocus);
        
    }
    
    void FocusEngine::sortFocusableNodesAsInOrder(const std::vector<cocos2d::CCNode *> & nodesInOrder, bool isHorizontally){
        
        //nullable accessor if out of range
        const auto nodeAtIndexL([&](const int index)->CCNode *{
            if (index>=0 && index<nodesInOrder.size())
                return nodesInOrder[index];
            return nullptr;
        });

        
        for (int i(0); i<nodesInOrder.size(); ++i) {
         
            CCNode * currNode(nodeAtIndexL(i));
            
            auto it(_focusables.find(currNode));
            if (it!=_focusables.end()) {
                CCNode * prevNode(nodeAtIndexL(i-1));
                CCNode * nextNode(nodeAtIndexL(i+1));
                
                if (isHorizontally) {
                    //favor horizontal layout
                    it->second._leftNode=prevNode;
                    it->second._rightNode=nextNode;
                    it->second._upLeftNode=prevNode;
                    it->second._upRightNode=nextNode;
                    it->second._downLeftNode=prevNode;
                    it->second._downRightNode=nextNode;
                }else{
                    //favor verical layout
                    it->second._downNode=prevNode;
                    it->second._upNode=nextNode;
                    it->second._downLeftNode=prevNode;
                    it->second._upLeftNode=nextNode;
                    it->second._downRightNode=prevNode;
                    it->second._upRightNode=nextNode;
                }
            }
        }
    }
    
    //sorting the matrix
    void FocusEngine::sortFocusableNodesByCurrentPositions(bool isHorizontalLayout, bool animateInitialFocus, bool isOneDimensional){
        struct WorldPositionNode{CCNode *node;CCPoint worldPos;};
        std::vector<WorldPositionNode> nodes;
        nodes.reserve(_focusables.size());
        for (const auto & p : _focusables)
            nodes.emplace_back(WorldPositionNode{p.first, p.first->getParent()->convertToWorldSpace(p.first->getPosition())});
        
        //nullable accessor if out of range
        const auto nodeAtIndexL([&](const int index)->CCNode *{
            if (index>=0 && index<nodes.size())
                return nodes[index].node;
            return nullptr;
        });
        
        //SORTING HORIZONTALLY (x)
        std::sort(nodes.begin(), nodes.end(), [](const WorldPositionNode & node1, const WorldPositionNode & node2){return node1.worldPos.x<node2.worldPos.x;});
        
        
        for (int i(0); i<nodes.size(); ++i) {
            CCNode * currNode(nodeAtIndexL(i));
            if (currNode) {
                auto it(_focusables.find(currNode));
                if (it!=_focusables.end()) {
                    it->second._leftNode=nodeAtIndexL(i-1);
                    it->second._rightNode=nodeAtIndexL(i+1);
                    
                    if (isHorizontalLayout) {
                        it->second._upLeftNode=nodeAtIndexL(i-1);
                        it->second._downLeftNode=nodeAtIndexL(i-1);
                        
                        it->second._upRightNode=nodeAtIndexL(i+1);
                        it->second._downRightNode=nodeAtIndexL(i+1);
                    }
                }
            }
        }
         
        
        //SORTING VERTICALLY (y)
        std::sort(nodes.begin(), nodes.end(), [](const WorldPositionNode & node1, const WorldPositionNode & node2){return node1.worldPos.y<node2.worldPos.y;});
        
        
        for (int i(0); i<nodes.size(); ++i) {
            CCNode * currNode(nodeAtIndexL(i));
            if (currNode) {
                auto it(_focusables.find(currNode));
                if (it!=_focusables.end()) {
                    it->second._downNode=nodeAtIndexL(i-1);
                    it->second._upNode=nodeAtIndexL(i+1);
                    
                    if (!isHorizontalLayout) {
                        it->second._downLeftNode=nodeAtIndexL(i-1);
                        it->second._downRightNode=nodeAtIndexL(i-1);
                        
                        it->second._upLeftNode=nodeAtIndexL(i+1);
                        it->second._upRightNode=nodeAtIndexL(i+1);
                    }
                }
            }
        }
        
        if (isOneDimensional) {
            if (isHorizontalLayout)
                for (auto & focusable : _focusables){
                    focusable.second._upNode=nullptr;
                    focusable.second._downNode=nullptr;
                }
            
            else
                for (auto & focusable : _focusables){
                    focusable.second._leftNode=nullptr;
                    focusable.second._rightNode=nullptr;
                }
        }
        
        //set current node if not available
        if (!_currentlyFocusedNode && !nodes.empty())
            setCurrentlyFocusedNode(nodes.front().node, true, animateInitialFocus);
        
        
    }
    
    
    //swipe
    cocos2d::CCNode * FocusEngine::focusSwipeBegan(const cocos2d::CCPoint & worldLocation){
        _swipeContext.lastSelectionLocation=worldLocation;
        _swipeContext.swipeEnabled=true;
        return _currentlyFocusedNode;
    }
    cocos2d::CCNode * FocusEngine::focusSwipeMoved(const cocos2d::CCPoint & worldLocation){
        _swipeContext.currentLocation=worldLocation;
        if (_swipeContext.swipeEnabled)
            _updateSelection();
        return _currentlyFocusedNode;
    }
    cocos2d::CCNode * FocusEngine::focusSwipeEnded(const cocos2d::CCPoint & worldLocation){
        _swipeContext.currentLocation=worldLocation;
        _updateSelection();
        return _currentlyFocusedNode;
    }

    void FocusEngine::_updateSelection(){
        const float distance(ccpDistance(_swipeContext.currentLocation, _swipeContext.lastSelectionLocation));

        const float horizontalDistance(_swipeContext.currentLocation.x-_swipeContext.lastSelectionLocation.x);

        const float verticalDistance((_swipeContext.currentLocation.y-_swipeContext.lastSelectionLocation.y)*_swipeContext.veritcalSwipeDistanceMultiplier);
        
        
        if (fabsf(distance * _swipeContext.kDiagonalDistanceMultiplier)>_swipeContext.kJumpingSwipeDistance) {
            //diagonals:
            
            if (_swipeContext.lastSelectionLocation.y<_swipeContext.currentLocation.y){
                if (_swipeContext.lastSelectionLocation.x>_swipeContext.currentLocation.x)
                    moveFocusUpLeft();
                else
                    moveFocusUpRight();
            }else{
                if (_swipeContext.lastSelectionLocation.x>_swipeContext.currentLocation.x)
                    moveFocusDownLeft();
                else
                    moveFocusDownRight();
            }
            
            //reset the horizontal and vetical distances distance
            _swipeContext.lastSelectionLocation=_swipeContext.currentLocation;
        }else if (fabsf(horizontalDistance)>_swipeContext.kJumpingSwipeDistance) {
            
            if (_swipeContext.lastSelectionLocation.x>_swipeContext.currentLocation.x)
                moveFocusLeft();
            else
                moveFocusRight();
                
            
            //reset the horizontal distance
            _swipeContext.lastSelectionLocation.x=_swipeContext.currentLocation.x;
        }else if (fabsf(verticalDistance)>_swipeContext.kJumpingSwipeDistance) {
            
            if (_swipeContext.lastSelectionLocation.y<_swipeContext.currentLocation.y)
                moveFocusUp();
            else
                moveFocusDown();
            
            //reset the horizontal distance
            _swipeContext.lastSelectionLocation.y=_swipeContext.currentLocation.y;
        }
        
    }
    
    //selection
    
    cocos2d::CCNode * FocusEngine::processDirectionalPress(const Press::Type & type, const bool animated){
        switch (type) {
            case Press::TypeLeftArrow:
                moveFocusLeft();
                break;
            case Press::TypeRightArrow:
                moveFocusRight();
                break;
            case Press::TypeUpArrow:
                moveFocusUp();
                break;
            case Press::TypeDownArrow:
                moveFocusDown();
                break;
            default:
                break;
        }
        return _currentlyFocusedNode;
    }
    
    cocos2d::CCNode * FocusEngine::_moveFocusToNode(const std::function<cocos2d::CCNode * (Focusable * currentlyFocusedNode)> & selectorHandle){
    
        if (_currentlyFocusedNode && selectorHandle) {
            auto it(_focusables.find(_currentlyFocusedNode));
            if (it!=_focusables.end()) {
                CCNode * nextNode(selectorHandle(&it->second));
                if (nextNode) {
                    setCurrentlyFocusedNode(nextNode, true, true);
                    if (_swipeContext.shoudDisableFurtherSwipesAfterFirstMoveOnSwipe)
                        _swipeContext.swipeEnabled=false;
                    return nextNode;
                }
            }
        }
        //bail:
        CCLog("no currently selected node!!!");
        return nullptr;

        
    }
    
    cocos2d::CCNode * FocusEngine::moveFocusUp(bool animated){
        return _moveFocusToNode([](Focusable * currentlyFocusedNode){return currentlyFocusedNode->_upNode;});
    }
    cocos2d::CCNode * FocusEngine::moveFocusUpLeft(bool animated){
        return _moveFocusToNode([](Focusable * currentlyFocusedNode){return currentlyFocusedNode->_upLeftNode;});
    }
    cocos2d::CCNode * FocusEngine::moveFocusUpRight(bool animated){
        return _moveFocusToNode([](Focusable * currentlyFocusedNode){return currentlyFocusedNode->_upRightNode;});
    }
    cocos2d::CCNode * FocusEngine::moveFocusDown(bool animated){
        return _moveFocusToNode([](Focusable * currentlyFocusedNode){return currentlyFocusedNode->_downNode;});
    }
    cocos2d::CCNode * FocusEngine::moveFocusDownLeft(bool animated){
        return _moveFocusToNode([](Focusable * currentlyFocusedNode){return currentlyFocusedNode->_downLeftNode;});
    }
    cocos2d::CCNode * FocusEngine::moveFocusDownRight(bool animated){
        return _moveFocusToNode([](Focusable * currentlyFocusedNode){return currentlyFocusedNode->_downRightNode;});
    }
    cocos2d::CCNode * FocusEngine::moveFocusRight(bool animated){
        return _moveFocusToNode([](Focusable * currentlyFocusedNode){return currentlyFocusedNode->_rightNode;});
    }
    cocos2d::CCNode * FocusEngine::moveFocusLeft(bool animated){
                return _moveFocusToNode([](Focusable * currentlyFocusedNode){return currentlyFocusedNode->_leftNode;});
    }
   
    std::string FocusEngine::focusIDForNode(cocos2d::CCNode * node){
        auto it(_focusables.find(node));
        if (it!=_focusables.end()) {
            const auto focusContext(it->second._focusContext);
            if (focusContext)
                return focusContext->focusID;
        }
        return "";
    }
    cocos2d::CCNode * FocusEngine::nodeWithFocusID(const std::string & focusID){
        for (const auto & p : _focusables) {
            if (p.second._focusContext && p.second._focusContext->focusID==focusID)
                return p.first;
        }
        return nullptr;
    }
    void FocusEngine::setCurrentlyFocusedNode(cocos2d::CCNode *node, bool withFocusAction, bool animated){
        auto it(_focusables.find(node));
        if (it!=_focusables.end()) {
            
            //unselect current node
            for (auto iter(_focusables.begin()); iter!=_focusables.end(); ++iter)
                if (iter!=it)
                    iter->second.focus(FocusStateNormal, animated);
            
            //select current node
            _currentlyFocusedNode=it->first;
            if (withFocusAction)
                it->second.focus(FocusStateFocused, animated);
            
        }else
            CCLog("Error! no such node registered!");
    }
    cocos2d::CCNode * FocusEngine::setCurrentlyFocusedNodePressed(bool pressed, bool animated){
        if (_currentlyFocusedNode){
            auto it(_focusables.find(_currentlyFocusedNode));
            if (it!=_focusables.end())
                it->second.focus(pressed? FocusStatePressed: FocusStateFocused, animated);
        }
        return _currentlyFocusedNode;
    }
    cocos2d::CCNode * FocusEngine::triggerCurrentlyFocusedNode(){
        if (_currentlyFocusedNode)
            triggerSelectionForFocusableNode(_currentlyFocusedNode);
        return _currentlyFocusedNode;
    }
    
    void FocusEngine::triggerSelectionForFocusableNode(cocos2d::CCNode * node){
        auto it(_focusables.find(node));
        if (it!=_focusables.end()){
            //trigger can destroy - so it is passed by copy
            auto triggerable(it->second.getTriggerable());
            triggerable();
        }
    }
    
#pragma mark focusable
    //ctor
    FocusEngine::Focusable::Focusable(cocos2d::CCNode * node, const focus_action_t & focusAction, const trigger_action_t & triggerAction, cocos2d::CCDictionary * focusContext)
    : _node(node)
    , _focusAction(focusAction)
    , _triggerAction(triggerAction)
    {
    
        if (focusContext) {
            
            using namespace mcb::PlatformSupport::Functions;
            
            _focusContext=pFocusContext(new FocusContext{
                                             stringForObjectKey(focusContext, "focusID"),
                                             boolForObjectKey(focusContext, "isFocused"),
                                             
                                             stringForObjectKey(focusContext, "up"),
                                             stringForObjectKey(focusContext, "upLeft"),
                                             stringForObjectKey(focusContext, "upRight"),
                                             stringForObjectKey(focusContext, "down"),
                                             stringForObjectKey(focusContext, "downLeft"),
                                             stringForObjectKey(focusContext, "downRight"),
                                             stringForObjectKey(focusContext, "left"),
                                             stringForObjectKey(focusContext, "right")
            });
            
        }
    
    }

    void FocusEngine::Focusable::focus(FocusState focusState, bool animated) const{
        if(focusState!=_focusState){
            const_cast<Focusable *>(this)->_focusState=focusState;
            if(_focusAction)
                _focusAction(const_cast<cocos2d::CCNode *>(_node), _focusState, animated);
        }
    }
    const std::function<void()> FocusEngine::Focusable::getTriggerable() const{
        CCNode * localNode(const_cast<cocos2d::CCNode *>(_node));
        auto localTriggerAction(_triggerAction);
        return [=]{
        if(localTriggerAction)
            localTriggerAction(localNode);
        };
    }
#pragma mark -
#pragma mark managed remote control mode
#pragma mark -
    FocusEngine::MCBTouchablePressable::MCBTouchablePressable(FocusEngine * focusEngine, int priority, bool swallowsInput){
        _focusEngine=focusEngine;
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -priority, swallowsInput);
        startListeningPresses(priority, swallowsInput);
    }
    FocusEngine::MCBTouchablePressable::~MCBTouchablePressable(){
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        stopListeningPresses();
    }
    
    //presses for the remote
    void FocusEngine::MCBTouchablePressable::pressBegan(const mcb::PlatformSupport::p_Press & press){_focusEngine->mcbPressBegan(press);}
    void FocusEngine::MCBTouchablePressable::pressChanged(const mcb::PlatformSupport::p_Press & press){_focusEngine->mcbPressChanged(press);}
    void FocusEngine::MCBTouchablePressable::pressEnded(const mcb::PlatformSupport::p_Press & press){_focusEngine->mcbPressEnded(press);}
    void FocusEngine::MCBTouchablePressable::pressCancelled(const mcb::PlatformSupport::p_Press & press){_focusEngine->mcbPressCancelled(press);}
    
    
    //touches for the focus engine
    bool FocusEngine::MCBTouchablePressable::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){return _focusEngine->mcbTouchBegan(pTouch, pEvent);}
    // optional
    void FocusEngine::MCBTouchablePressable::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){_focusEngine->mcbTouchMoved(pTouch, pEvent);}
    void FocusEngine::MCBTouchablePressable::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){_focusEngine->mcbTouchEnded(pTouch, pEvent);}
    void FocusEngine::MCBTouchablePressable::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){_focusEngine->mcbTouchCancelled(pTouch, pEvent);}
    
    
#pragma mark -
    
    
    void FocusEngine::beginManagedInput(int priority, bool swallowsInput){
        _mcbTouchablePressable=std::make_shared<MCBTouchablePressable>(this, priority, swallowsInput);
    }
    void FocusEngine::endManagedInput(){
        _mcbTouchablePressable=nullptr;
    }
    
#pragma mark remote control event receivers (you need to call super once override for managed input to work)
    
    void FocusEngine::mcbPressBegan(const p_Press & press){
        processDirectionalPress(press->type);
        if(press->type==Press::TypeSelect)
            setCurrentlyFocusedNodePressed(true, _managedInputAnimated);
        mcbRemoteButtonPressed(press);
    }
    void FocusEngine::mcbPressChanged(const p_Press & press){
        
    }
    void FocusEngine::mcbPressEnded(const p_Press & press){
        if(press->type==Press::TypeSelect){
            setCurrentlyFocusedNodePressed(false, _managedInputAnimated);
            triggerCurrentlyFocusedNode();
        }
    }
    void FocusEngine::mcbPressCancelled(const p_Press & press){
        if(press->type==Press::TypeSelect)
            setCurrentlyFocusedNodePressed(false, _managedInputAnimated);
    }
    
    bool FocusEngine::mcbTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
        focusSwipeBegan(pTouch->getLocation());
        return true;
    }
    void FocusEngine::mcbTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
        focusSwipeMoved(pTouch->getLocation());
    }
    void FocusEngine::mcbTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
        focusSwipeEnded(pTouch->getLocation());
    }
    void FocusEngine::mcbTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
        focusSwipeEnded(pTouch->getLocation());
    }
    
}}
