//
//  mcbBundle.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/12/14.
//
//

#include "mcbBundle.h"
#include "mcbPlatformSupportFunctions.h"
#include "mcbBundleCatalog.h"

namespace mcb{namespace PlatformSupport{namespace network{
    pBundle Bundle::create(){
        return pBundle(new Bundle);
    }
   
#pragma mark user metadata
    bool Bundle::boolForKey(const std::string & key, bool defaultVal) const{
        auto it(_userMetadata.find(key));
        if (it!=_userMetadata.end())
            return PlatformSupport::Functions::boolFromString(it->second);
        return defaultVal;
    }
    void Bundle::setBoolForKey(const std::string & key, bool value){
        _userMetadata[key]=PlatformSupport::Functions::stringFromBool(value);
    }
    
    int Bundle::intForKey(const std::string & key, int defaultVal) const{
        auto it(_userMetadata.find(key));
        if (it!=_userMetadata.end())
            return PlatformSupport::Functions::intFromString(it->second);
        return defaultVal;
    }
    void Bundle::setIntForKey(const std::string & key, int value){
        _userMetadata[key]=PlatformSupport::Functions::t_to_string(value);
    }
    
    long Bundle::longForKey(const std::string & key, long defaultVal) const{
        auto it(_userMetadata.find(key));
        if (it!=_userMetadata.end())
            return PlatformSupport::Functions::longFromString(it->second);
        return defaultVal;
    }
    void Bundle::setLongForKey(const std::string & key, long value){
        _userMetadata[key]=PlatformSupport::Functions::t_to_string(value);
    }
    
    float Bundle::floatForKey(const std::string & key, float defaultVal) const{
        auto it(_userMetadata.find(key));
        if (it!=_userMetadata.end())
            return PlatformSupport::Functions::floatFromString(it->second);
        return defaultVal;
    }
    void Bundle::setFloatForKey(const std::string & key, float value){
        _userMetadata[key]=PlatformSupport::Functions::t_to_string(value);
    }
    std::string Bundle::stringForKey(const std::string & key, const std::string & defaultVal) const{
        auto it(_userMetadata.find(key));
        if (it!=_userMetadata.end())
            return it->second;
        return defaultVal;
    }
    void Bundle::setStringForKey(const std::string & key, const std::string & value){
        _userMetadata[key]=value;
    }
    
    void Bundle::serializeUserData(){
        BundleCatalog::sharedInstance()->_serializeBundles();
    }
}}}