//
//  mcbFocusEngine.cpp
//  EndlessAlphabet
//
//  Created by Radif Sharafullin on 1/19/16.
//
//

#include "mcbFocusEngine.hpp"
namespace mcb{namespace PlatformSupport{
    using namespace cocos2d;
    void FocusEngine::addFocusableNode(cocos2d::CCNode * node, const std::function<void(cocos2d::CCNode * node, const bool isFocused, bool animated)> & focusAction, const std::function<void(cocos2d::CCNode * node)> & triggerAction){
        removeFocusableNode(node);
        _focusables.insert({node, Focusable{node, focusAction, triggerAction}});
    }
    
    void FocusEngine::removeFocusableNode(cocos2d::CCNode * node){
        auto it(_focusables.find(node));
        if (it!=_focusables.end())
            _focusables.erase(it);
    }
    
    void FocusEngine::removeAllFocusableNodes(){
        _currentlySelectedNode=nullptr;
        _focusables.erase(_focusables.begin(), _focusables.end());
    }
    
    //sorting the matrix
    void FocusEngine::sortFocusableNodesByCurrentPositions(){
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
                }
            }
        }
        
        //set current node if not available
        if (!_currentlySelectedNode && !nodes.empty())
            setCurrentlyFocusedNode(nodes.front().node, true, false);
        
        
    }
    
    
    //swipe
    cocos2d::CCNode * FocusEngine::focusSwipeBegan(const cocos2d::CCPoint & worldLocation){
        _swipeContext.lastSelectionLocation=worldLocation;
        return _currentlySelectedNode;
    }
    cocos2d::CCNode * FocusEngine::focusSwipeMoved(const cocos2d::CCPoint & worldLocation){
        _swipeContext.currentLocation=worldLocation;
        _updateSelection();
        return _currentlySelectedNode;
    }
    cocos2d::CCNode * FocusEngine::focusSwipeEnded(const cocos2d::CCPoint & worldLocation){
        _swipeContext.currentLocation=worldLocation;
        _updateSelection();
        return _currentlySelectedNode;
    }

    void FocusEngine::_updateSelection(){
        const float distance(ccpDistance(_swipeContext.currentLocation, _swipeContext.lastSelectionLocation));

        const float horizontalDistance(_swipeContext.currentLocation.x-_swipeContext.lastSelectionLocation.x);

        const float verticalDistance(_swipeContext.currentLocation.y-_swipeContext.lastSelectionLocation.y);
        
        if (fabsf(horizontalDistance)>_swipeContext.kJumpingSwipeDistance) {
            
            if (_swipeContext.lastSelectionLocation.x>_swipeContext.currentLocation.x)
                moveFocusLeft();
            else
                moveFocusRight();
                
            
            //reset the horizontal distance
            _swipeContext.lastSelectionLocation.x=_swipeContext.currentLocation.x;
        }
        if (fabsf(verticalDistance)>_swipeContext.kJumpingSwipeDistance) {
            
            if (_swipeContext.lastSelectionLocation.y<_swipeContext.currentLocation.y)
                moveFocusUp();
            else
                moveFocusDown();
            
            //reset the horizontal distance
            _swipeContext.lastSelectionLocation.y=_swipeContext.currentLocation.y;
        }
        
    }
    
    //selection
    
    cocos2d::CCNode * FocusEngine::moveFocusRight(bool animated){
        if (_currentlySelectedNode) {
            auto it(_focusables.find(_currentlySelectedNode));
            if (it!=_focusables.end()) {
                CCNode * nextNode(it->second._rightNode);
                if (nextNode) {
                    setCurrentlyFocusedNode(nextNode, true, true);
                    return nextNode;
                }
            }
        }
        //bail:
        CCLog("no currently selected node!!!");
        return nullptr;
    }
    cocos2d::CCNode * FocusEngine::moveFocusLeft(bool animated){
        if (_currentlySelectedNode) {
            auto it(_focusables.find(_currentlySelectedNode));
            if (it!=_focusables.end()) {
                CCNode * nextNode(it->second._leftNode);
                if (nextNode) {
                    setCurrentlyFocusedNode(nextNode, true, true);
                    return nextNode;
                }
            }
        }
        //bail:
        CCLog("no currently selected node!!!");
        return nullptr;
    }
    cocos2d::CCNode * FocusEngine::moveFocusUp(bool animated){
        if (_currentlySelectedNode) {
            auto it(_focusables.find(_currentlySelectedNode));
            if (it!=_focusables.end()) {
                CCNode * nextNode(it->second._upNode);
                if (nextNode) {
                    setCurrentlyFocusedNode(nextNode, true, true);
                    return nextNode;
                }
            }
        }
        //bail:
        CCLog("no currently selected node!!!");
        return nullptr;
    }
    cocos2d::CCNode * FocusEngine::moveFocusDown(bool animated){
        if (_currentlySelectedNode) {
            auto it(_focusables.find(_currentlySelectedNode));
            if (it!=_focusables.end()) {
                CCNode * nextNode(it->second._downNode);
                if (nextNode) {
                    setCurrentlyFocusedNode(nextNode, true, true);
                    return nextNode;
                }
            }
        }
        //bail:
        CCLog("no currently selected node!!!");
        return nullptr;
    }
    
    void FocusEngine::setCurrentlyFocusedNode(cocos2d::CCNode *node, bool withFocusAction, bool animated){
        auto it(_focusables.find(node));
        if (it!=_focusables.end()) {
            
            //unselect current node
            for (auto iter(_focusables.begin()); iter!=_focusables.end(); ++iter)
                if (iter!=it)
                    iter->second.focus(false, animated);
            
            //select current node
            _currentlySelectedNode=it->first;
            if (withFocusAction)
                it->second.focus(true, animated);
            
        }else
            CCLog("Error! no such node registered!");
    }
    cocos2d::CCNode * FocusEngine::triggerCurrentlyFocusedNode(){
        if (_currentlySelectedNode)
            triggerSelectionForFocusableNode(_currentlySelectedNode);
        return _currentlySelectedNode;
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
    FocusEngine::Focusable::Focusable(cocos2d::CCNode * node, const std::function<void(cocos2d::CCNode * node, const bool isFocused, bool animated)> & focusAction, const std::function<void(cocos2d::CCNode * node)> & triggerAction)
    : _node(node)
    , _focusAction(focusAction)
    , _triggerAction(triggerAction)
    {}

    void FocusEngine::Focusable::focus(bool m_isFocused, bool animated) const{
        if(m_isFocused!=_isFocused){
            const_cast<Focusable *>(this)->_isFocused=m_isFocused;
            if(_focusAction)
                _focusAction(const_cast<cocos2d::CCNode *>(_node), _isFocused, animated);
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
}}
