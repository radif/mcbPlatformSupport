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
    void FocusEngine::init(){
        
    }
    
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
        _focusables.erase(_focusables.begin(), _focusables.end());
    }
    
    //sorting the matrix
    void FocusEngine::sortByCurrentPositions(){
        struct WorldPositionNode{CCNode *node;CCPoint worldPos;};
        std::vector<WorldPositionNode> nodes;
        nodes.reserve(_focusables.size());
        for (const auto & p : _focusables)
            nodes.emplace_back(WorldPositionNode{p.first, p.first->getParent()->convertToWorldSpace(p.first->getPosition())});
        
        //nullable accessor if exceeds index, !!!does not check if less than 0!
        const auto nodeAtIndexL([&](const int index)->CCNode *{
            if (index<nodes.size())
                return nodes[index].node;
            return nullptr;
        });
        
        //SORTING HORIZONTALLY (x)
        std::sort(nodes.begin(), nodes.end(), [](const WorldPositionNode & node1, const WorldPositionNode & node2){return node1.worldPos.x>node2.worldPos.x;});
        
        
        for (int i(0); i<nodes.size(); ++i) {
            CCNode * currNode(nodeAtIndexL(i));
            if (currNode) {
                auto it(_focusables.find(currNode));
                if (it!=_focusables.end()) {
                    if (i==0)
                        it->second._leftNode=nullptr;
                    it->second._rightNode=nodeAtIndexL(i+1);
                }
            }
        }
         
        
        //SORTING VERTICALLY (y)
        std::sort(nodes.begin(), nodes.end(), [](const WorldPositionNode & node1, const WorldPositionNode & node2){return node1.worldPos.y>node2.worldPos.y;});
        
        
        for (int i(0); i<nodes.size(); ++i) {
            CCNode * currNode(nodeAtIndexL(i));
            if (currNode) {
                auto it(_focusables.find(currNode));
                if (it!=_focusables.end()) {
                    if (i==0)
                        it->second._downNode=nullptr;
                    it->second._upNode=nodeAtIndexL(i+1);
                }
            }
        }
        
    }
    
    //selection
    
    cocos2d::CCNode * FocusEngine::moveSelectionRight(bool animated){
        if (_currentlySelectedNode) {
            auto it(_focusables.find(_currentlySelectedNode));
            if (it!=_focusables.end()) {
                CCNode * nextNode(it->second._rightNode);
                if (nextNode) {
                    setCurrentlySelectedNode(nextNode, true, true);
                    return nextNode;
                }
            }
        }
        //bail:
        CCLog("no currently selected node!!!");
        return nullptr;
    }
    cocos2d::CCNode * FocusEngine::moveSelectionLeft(bool animated){
        if (_currentlySelectedNode) {
            auto it(_focusables.find(_currentlySelectedNode));
            if (it!=_focusables.end()) {
                CCNode * nextNode(it->second._leftNode);
                if (nextNode) {
                    setCurrentlySelectedNode(nextNode, true, true);
                    return nextNode;
                }
            }
        }
        //bail:
        CCLog("no currently selected node!!!");
        return nullptr;
    }
    cocos2d::CCNode * FocusEngine::moveSelectionUp(bool animated){
        if (_currentlySelectedNode) {
            auto it(_focusables.find(_currentlySelectedNode));
            if (it!=_focusables.end()) {
                CCNode * nextNode(it->second._upNode);
                if (nextNode) {
                    setCurrentlySelectedNode(nextNode, true, true);
                    return nextNode;
                }
            }
        }
        //bail:
        CCLog("no currently selected node!!!");
        return nullptr;
    }
    cocos2d::CCNode * FocusEngine::moveSelectionDown(bool animated){
        if (_currentlySelectedNode) {
            auto it(_focusables.find(_currentlySelectedNode));
            if (it!=_focusables.end()) {
                CCNode * nextNode(it->second._downNode);
                if (nextNode) {
                    setCurrentlySelectedNode(nextNode, true, true);
                    return nextNode;
                }
            }
        }
        //bail:
        CCLog("no currently selected node!!!");
        return nullptr;
    }
    
    void FocusEngine::setCurrentlySelectedNode(cocos2d::CCNode *node, bool withFocusAction, bool animated){
        auto it(_focusables.find(node));
        if (it!=_focusables.end() && !it->second.isFocused && node!=_currentlySelectedNode && node!=nullptr) {
            //unselect current node
            std::for_each(_focusables.begin(), _focusables.end(), [&](const std::pair<cocos2d::CCNode *, Focusable> & p){
                    //unselect is always with focus action
                    p.second.focus(false, animated);
            });
            
            //select current node
            _currentlySelectedNode=it->first;
            if (withFocusAction)
                it->second.focus(true, animated);
            
        }else
            CCLog("Error! no such node registered!");
    }
    cocos2d::CCNode * FocusEngine::triggerCurrentSelection(){
        if (_currentlySelectedNode)
            triggerSelectionForNode(_currentlySelectedNode);
        return _currentlySelectedNode;
    }
    
    void FocusEngine::triggerSelectionForNode(cocos2d::CCNode * node){
        auto it(_focusables.find(node));
        if (it!=_focusables.end())
            it->second.trigger();
    }
}}
