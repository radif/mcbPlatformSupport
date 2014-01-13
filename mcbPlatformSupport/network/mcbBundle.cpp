//
//  mcbBundle.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/12/14.
//
//

#include "mcbBundle.h"
#include "mcbPlatformSupportFunctions.h"

namespace mcb{namespace PlatformSupport{namespace network{
    pBundle Bundle::create(){
        return pBundle(new Bundle);
    }
    pBundle Bundle::createWithJSONRepresentation(const std::string & JSONString){
        pBundle retVal(new Bundle);
        
        return retVal;
    }
    std::string Bundle::JSONRepresentation() const{
        std::string retVal;

        //building JSON
        retVal+="{";
        auto appendKeyValL([](std::string & JSONString, const std::string & key, const std::string & val, bool isValueString, bool isLast){
            std::string valQuotes(isValueString?"\"":"");
            JSONString+="{\""+key+"\":"+valQuotes+val+valQuotes+"}";
            if (!isLast)
                JSONString+=",";
        });
        
        
        appendKeyValL(retVal, "identifier", _identifier, true, false);
        appendKeyValL(retVal, "localPath", _localPath, true, false);
        appendKeyValL(retVal, "title", _title, true, false);
        appendKeyValL(retVal, "version", PlatformSupport::Functions::t_to_string(_version) , false, false);
        appendKeyValL(retVal, "downloadTimestamp", PlatformSupport::Functions::t_to_string(_downloadTimestamp) , false, false);
        
        //std::string statusString
        
        //user metadata
        {
            std::string metadataJSON;
            for (auto it(_userMetadata.cbegin()); it!=_userMetadata.cend(); ++it) {
                auto nextIt(it);++nextIt;
                appendKeyValL(metadataJSON, (*it).first, (*it).second, true, nextIt==_userMetadata.cend());
            }
            retVal+="{\"userMetadata\":["+metadataJSON+"]}";
        }
        retVal+="}";
        return retVal;
    }
    
    
#pragma mark user metadata
    bool Bundle::boolForKey(const std::string & key, bool defaultVal) const{
        auto it(_userMetadata.find(key));
        if (it!=_userMetadata.end())
            return PlatformSupport::Functions::boolFromString((*it).second);
        return defaultVal;
    }
    void Bundle::setBoolForKey(const std::string & key, bool value){
        _userMetadata[key]=PlatformSupport::Functions::stringFromBool(value);
    }
    
    int Bundle::intForKey(const std::string & key, int defaultVal) const{
        auto it(_userMetadata.find(key));
        if (it!=_userMetadata.end())
            return PlatformSupport::Functions::intFromString((*it).second);
        return defaultVal;
    }
    void Bundle::setIntForKey(const std::string & key, int value){
        _userMetadata[key]=PlatformSupport::Functions::t_to_string(value);
    }
    
    float Bundle::floatForKey(const std::string & key, float defaultVal) const{
        auto it(_userMetadata.find(key));
        if (it!=_userMetadata.end())
            return PlatformSupport::Functions::floatFromString((*it).second);
        return defaultVal;
    }
    void Bundle::setFloatForKey(const std::string & key, float value){
        _userMetadata[key]=PlatformSupport::Functions::t_to_string(value);
    }
    std::string Bundle::stringForKey(const std::string & key, const std::string & defaultVal) const{
        auto it(_userMetadata.find(key));
        if (it!=_userMetadata.end())
            return (*it).second;
        return defaultVal;
    }
    void Bundle::setStringForKey(const std::string & key, const std::string & value){
        _userMetadata[key]=value;
    }
}}}