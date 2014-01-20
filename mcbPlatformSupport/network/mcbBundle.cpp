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
    pBundle Bundle::createByCopy(const pBundle & existing){
        pBundle retVal(create());
        
        retVal->_identifier=existing->_identifier;
        retVal->_version=existing->_version;
        retVal->_status=existing->_status;
        retVal->_downloadTimestamp=existing->_downloadTimestamp;
        retVal->_localPath=existing->_localPath;
        retVal->_remoteURL=existing->_remoteURL;
        retVal->_title=existing->_title;
        retVal->_preshipped=existing->_preshipped;
        retVal->_userMetadata=existing->_userMetadata;
        retVal->_contentLabels=existing->_contentLabels;
        
        return retVal;
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
        pBundle b(BundleCatalog::sharedInstance()->bundleByIdentifier(_identifier));
        
        //in case, this bundle is outdated and not part of circulating sdk, it will sync both metadatas and save
        if (b && b.get()!=this) {
            for (const auto &p : _userMetadata)
                b->_userMetadata[p.first]=p.second;
            
            _userMetadata=b->_userMetadata;
        }
        BundleCatalog::sharedInstance()->_serializeBundles();
    }
    bool Bundle::isDisjointedFromCatalog() const{
        pBundle b(BundleCatalog::sharedInstance()->bundleByIdentifier(_identifier));
        return b.get()!=this;
    }
    bool Bundle::hasLocalAssets() const{
        if (_localPath.empty())
            return false;
        return Functions::fileExists(_localPath);
    }
}}}