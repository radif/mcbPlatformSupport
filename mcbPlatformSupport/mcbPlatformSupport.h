//
//  PlatformSupport.h
//  mcb
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#ifndef __mcb__PlatformSupport__
#define __mcb__PlatformSupport__

#include "cocos2d.h"
#include "mcbPlatformSupportConstants.h"

//safer version of the same macro, comes with 2 parts: "cdaForEachBegin" and "cdaForEachEnd"
#define mcbForEachBegin(CCType ,__object__, __array__)                                              \
{                                                                                                   \
CCType __object__;                                                                                  \
if (__array__ && __array__->data->num > 0)                                                          \
for(cocos2d::CCObject** arr = __array__->data->arr, **end = __array__->data->arr + __array__->data->num-1;	\
arr <= end && ((__object__ = (CCType)*arr) != 0/* || true*/);                                    \
arr++){

#define mcbForEachEnd }}

namespace mcb{namespace PlatformSupport{
    typedef enum DeviceType{
        DeviceTypeUnknown=0,
        DeviceTypeTablet=1,
        DeviceTypeHandheld=2,
        DeviceTypeTV=3,
    }DeviceType;
    
    void setVisibleScreenRect(const cocos2d::CCRect & rect);
    cocos2d::CCRect getVisibleScreenRect();
    
    cocos2d::CCRect getEdgeProtectedScreenRect();
    void setEdgeProtectedScreenRect(cocos2d::CCRect rect);
    
    
    float getScreenScaleRatio();
    void setScreenScaleRatio(float screenScaleRatio);
    
    void setDeviceIdiom(const std::string & newDeviceIdiom);
    std::string getDeviceIdiom();
    
    void setDeviceType(const DeviceType & devType);
    DeviceType getDeviceType();
    bool isTVPlatform();
    bool isOn16x9Screen();
    
    void setIsLowPerformingDevice(bool isLowPerformingDevice);
    bool isLowPerformingDevice();
    
    void setIsIPhoneX(bool isIPhoneX);
    bool isIPhoneX();
    void setIPhoneXScaleMultiplier(float iPhoneXScaleMultiplier);
    float iPhoneXScaleMultiplier();
    
    std::string getDeviceSuffix();
    
    cocos2d::CCPoint getScreenCenter();
    
    void iterateArray(cocos2d::CCArray * arr, std::function<void(cocos2d::CCObject *item, unsigned idx, bool & stop)>block);
    
    cocos2d::CCDictionary *dictionaryFromPlist(const std::string & plistFileName);
    
    //paths
    typedef std::map<std::string, std::string> path_tokens_t;
    std::string resolvePath(std::string inPath, const std::string & localDirectory="", std::map<std::string, std::string> * userTokens=nullptr);
    void setCustomResolvePathFunction(const std::function<void(std::string & inPath)> & resolvePathFunction);
    
    void setSharedBundlePath(const std::string & gbpath);
    void addTokenForPath(const std::string token, const std::string path);

    std::string getSharedBundlePath();
    
    std::string _platformSpecificDocsPath();
    std::string _platformSpecificCachesPath();
    std::string _platformSpecificLibPath();
    std::string _platformSpecificBundlePath();
    
    //time
    long long hostTime();
}}
#endif /* defined(__mcb__PlatformSupport__) */
