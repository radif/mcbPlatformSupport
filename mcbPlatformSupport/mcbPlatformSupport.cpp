//
//  PlatformSupport.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#include "mcbPlatformSupport.h"
#include "mcbNumber.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "mcbPlatformSupportFunctions.h"

#include <chrono>


/*
 *To compile this file using gcc you can type
 *gcc `xml2-config --cflags --libs` -o xmlexample libxml2-example.c
 */

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */




using namespace cocos2d;

namespace mcb{namespace PlatformSupport{
    static cocos2d::CCRect _visibleScreenRect=cocos2d::CCRectZero;
    static cocos2d::CCRect _edgeProtectedScreenRect=cocos2d::CCRectZero;
    static std::string _deviceIdiom;
    static std::string _sharedBundlePath;
    static std::unique_ptr<path_tokens_t> _pathTokens=nullptr;
    static float _screenScaleRatio=1.0f;
    static DeviceType _devType=DeviceTypeUnknown;
    static bool _isLowPerformingDevice = false;
    bool _isiPhoneX = false;
    float _iPhoneXScaleMultiplier = 1.f;
    
    static std::function<void(std::string & inPath)> _customResolvePathFunction=nullptr;
    
    void setVisibleScreenRect(const cocos2d::CCRect & rect){_visibleScreenRect=rect;}
    cocos2d::CCRect getVisibleScreenRect(){
        if (!_visibleScreenRect.size.width && !_visibleScreenRect.size.height){
            _visibleScreenRect=cocos2d::CCRectZero;
            _visibleScreenRect.size=cocos2d::CCDirector::sharedDirector()->getWinSize();
        }
        return _visibleScreenRect;
    }
    
    cocos2d::CCRect getEdgeProtectedScreenRect(){ return _edgeProtectedScreenRect; }
    void setEdgeProtectedScreenRect(cocos2d::CCRect rect){_edgeProtectedScreenRect=rect;}
    
    
    float getScreenScaleRatio(){return _screenScaleRatio;}
    void setScreenScaleRatio(float screenScaleRatio){_screenScaleRatio=screenScaleRatio;}
    
    void setIsLowPerformingDevice(bool isLowPerformingDevice){_isLowPerformingDevice = isLowPerformingDevice;}
    bool isLowPerformingDevice(){return isLowPerformingDevice;}
    
    void setIsIPhoneX(bool isIPhoneX){_isiPhoneX = isIPhoneX;}
    bool isIPhoneX(){return _isiPhoneX;}
    void setIPhoneXScaleMultiplier(float iPhoneXScaleMultiplier){_iPhoneXScaleMultiplier = iPhoneXScaleMultiplier;}
    float iPhoneXScaleMultiplier(){return _iPhoneXScaleMultiplier;}
    
    void setDeviceIdiom(const std::string & newDeviceIdiom){_deviceIdiom=newDeviceIdiom;}
    std::string getDeviceIdiom(){
        if (_deviceIdiom=="") {
            TargetPlatform tp(CCApplication::sharedApplication()->getTargetPlatform());
            switch (tp) {
                case kTargetIpad:
                    _deviceIdiom="iPad";
                    break;
                case kTargetIphone:
                    _deviceIdiom="iPhone";
                    break;
                case kTargetAndroid:
                    _deviceIdiom="Android";
                    break;
                    
                    //TODO: more logic
                default:
                    break;
            }
        }
        return _deviceIdiom;
    }
    
    void setDeviceType(const DeviceType & devType){_devType=devType;}
    DeviceType getDeviceType(){
        
        if (_devType==DeviceTypeUnknown) {
            if (isTVPlatform())
                _devType=DeviceTypeTV;
            else{
            
                TargetPlatform tp(CCApplication::sharedApplication()->getTargetPlatform());
                switch (tp) {
                    case kTargetIpad:
                        _devType=DeviceTypeTablet;
                        break;
                    case kTargetIphone:
                        _devType=DeviceTypeHandheld;
                        break;
                        
                        //TODO: more logic
                    default:
                        break;
                }
            }
        }
        return _devType;
        
    }
    std::string getDeviceSuffix(){
        switch (_devType) {
            case DeviceTypeHandheld:
                if(_isiPhoneX)
                    return "-X";
                return "-hh";
                break;
            case DeviceTypeTablet:
                return "-tb";
                break;
            case DeviceTypeTV:
                return "-tv";
                break;
            default:
                return "-tb";
                break;
        }
        return "-tb";
    }
    bool isOn16x9Screen(){
        const float screenRatio(_visibleScreenRect.size.height/_visibleScreenRect.size.width);
        return screenRatio<.57f;
    }
    
    
    
    cocos2d::CCPoint getScreenCenter(){return {_visibleScreenRect.origin.x+_visibleScreenRect.size.width*.5f, _visibleScreenRect.origin.y+_visibleScreenRect.size.height*.5f};}
    
    
    
    void setSharedBundlePath(const std::string & shpath){_sharedBundlePath=resolvePath(shpath);}
    void addTokenForPath(const std::string token, const std::string path){if (!_pathTokens) _pathTokens=std::unique_ptr<path_tokens_t>(new path_tokens_t); (*_pathTokens)[token]=path;}

    std::string getSharedBundlePath(){return _sharedBundlePath;}
    
    void iterateArray(cocos2d::CCArray * arr, std::function<void(cocos2d::CCObject *item, unsigned idx, bool & stop)>block){
        if (!block)
            return;
        const unsigned count(arr->count());
        bool stop(false);
        for (int index(0); index<count; ++index) {
            block(arr->objectAtIndex(index),index, stop);
            if (stop)
                break;
        }
    }
    
#pragma mark path resolution
    void _resolveSystemPaths(std::string & inPath) {
        //shared
        PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, "$(SHARED)", getSharedBundlePath());
        // bundle
        PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, "$(BUNDLE)", _platformSpecificBundlePath());
        //documents
        PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, "$(DOCUMENTS)", _platformSpecificDocsPath());
        //cache
        PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, "$(CACHES)", _platformSpecificCachesPath());
        //Library
        PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, "$(LIBRARY)", _platformSpecificLibPath());
        //General Assets
        PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, "$(ASSETS)", _platformSpecificBundlePath()+"/assets");
        //Downloads
        //....
        if (_pathTokens)
            for (const std::pair<std::string, std::string> & pair: *_pathTokens)
                PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, pair.first, pair.second);
    }
    
    void _resolveSystemAndLocalPaths(std::string & inPath, const std::string & localDirectory){
        _resolveSystemPaths(inPath);
        std::string pathWithDeletedLastComponent(PlatformSupport::Functions::stringByDeletingLastPathComponent(localDirectory));
        PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, "$(LOCAL)..", pathWithDeletedLastComponent);
        PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, "$(LOCAL)/..", pathWithDeletedLastComponent);
        PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, "$(LOCAL)", localDirectory);
    }
    void setCustomResolvePathFunction(const std::function<void(std::string & inPath)> & resolvePathFunction){
        _customResolvePathFunction=resolvePathFunction;
    }
    std::string resolvePath(std::string inPath, const std::string & localDirectory, std::map<std::string, std::string> * userTokens){
        if (_customResolvePathFunction)
            _customResolvePathFunction(inPath);
        
        Functions::_removeLastSlashInPath(inPath);
        if (localDirectory.empty())
            _resolveSystemPaths(inPath);
        else
            _resolveSystemAndLocalPaths(inPath, localDirectory);
        
        if (userTokens)
            for (const std::pair<std::string, std::string> & pair: *userTokens)
                PlatformSupport::Functions::replaceOccurrencesOfStringByString(inPath, pair.first, pair.second);
        
        return inPath;
    }
#pragma mark -
#pragma mark parsing
    
    static cocos2d::CCObject *allocValueForNode(xmlNode *n)
    {
        cocos2d::CCObject *retVal(0);
        
        CCAssert(n->type==XML_ELEMENT_NODE, "Plist may not be valid!");
        
        const char *name = (const char *)n->name;
        const char *content = (n->children) ? (const char *)n->children->content : 0;
        
        
        if (strcmp(name, "true") == 0) {
            retVal = new Number(1);
        }else if (strcmp(name, "false") == 0) {
            retVal = new Number(0);
        }else if (strcmp(name, "string") == 0) {
            if (!content) content = "";
            retVal = new cocos2d::CCString(content);
        }else if (strcmp(name, "real") == 0) {
            retVal = new Number(atof(content));
        }else if (strcmp(name, "integer") == 0) {
            retVal = new Number(atoi(content));
        }else if (strcmp(name, "dict") == 0) {
            cocos2d::CCDictionary *d = new cocos2d::CCDictionary();
            xmlNode *curr = n->children;
            while (curr) {
                if (strcmp((const char *)curr->name, "key") == 0) {
                    std::string key = std::string(reinterpret_cast<const char *>(curr->children->content));
                    curr = curr->next;
                    cocos2d::CCObject *obj = allocValueForNode(curr);
                    d->setObject(obj, key);
                    obj->release();
                    curr = curr->next;
                }
            }
            retVal = d;
        }else if (strcmp(name, "array") == 0) {
            cocos2d::CCArray *a = new cocos2d::CCArray();
            a->init();
            xmlNode *curr = n->children;
            while (curr) {
                cocos2d::CCObject *obj = allocValueForNode(curr);
                curr = curr->next;
                a->addObject(obj);
                obj->release();
            }
            retVal = a;
        }
        
        assert(retVal);
        return retVal;
    }
    
    static void
    print_element_names(xmlNode * a_node, std::string indentation)
    {
        xmlNode *cur_node(0);
        for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
            if (cur_node->type == XML_ELEMENT_NODE) {
                printf("%snode type: Element, name: %s\n", indentation.c_str(), cur_node->name);
            } else if (cur_node->type == XML_TEXT_NODE) {
                printf("%snode type: Text, content: %s(%lu)\n", indentation.c_str(), cur_node->content, strlen((const char *)cur_node->content));
            }
            print_element_names(cur_node->children, indentation+"\t");
        }
    }
    
    cocos2d::CCDictionary *dictionaryFromPlist(const std::string & plistFileName){
        xmlDoc *doc(0);
        xmlNode *root_element(0);
        
        /*
         * this initialize the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        LIBXML_TEST_VERSION
        
        /*parse the file and get the DOM */
        
        // on android i seem to have to go through ccfileutils to get the data, may as well always do it
        
        
        
        unsigned long  uSize(0);
        xmlChar * xmlBuff(cocos2d::CCFileUtils::sharedFileUtils()->getFileData(plistFileName.c_str(), "r", &uSize));
        if (uSize)
            doc = xmlReadMemory((const char *)xmlBuff, uSize, "", 0, XML_PARSE_NOBLANKS);
        
        CC_SAFE_DELETE_ARRAY(xmlBuff);
        
        //could not parse the file pFileName!
        assert(doc);
        
        /*Get the root element node */
        root_element = xmlDocGetRootElement(doc);
        //  print_element_names(root_element, "");
        cocos2d::CCDictionary *d = (cocos2d::CCDictionary *) allocValueForNode(root_element->children);
        d->autorelease();
        
        /*free the document */
        xmlFreeDoc(doc);
        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();
        
        return d;
    }
    
    long long hostTime(){
        auto duration = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }
    
}}
