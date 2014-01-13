//
//  mcbBundle.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/12/14.
//
//

#ifndef __SoundSurfer__mcbBundle__
#define __SoundSurfer__mcbBundle__

#include <iostream>
#include <memory>
#include <map>

namespace mcb{namespace PlatformSupport{namespace network{
    class BundleFetcher;
    class Bundle;
    typedef std::shared_ptr<Bundle> pBundle;
    class Bundle{
        friend BundleFetcher;
        float _version=0.f;
        long long _downloadTimestamp=0;
        std::string _identifier;
        std::string _localPath;//tokenized, need to process with mcbPath
        std::string _remoteURL;
        std::string _title;
        Bundle()=default;//avoiding make_shared in sake of hiding constructor to allow the creation of object through create func
        std::map<std::string, std::string> _userMetadata;
    public:
        typedef enum {StatusUndefined, StatusAvailable, StatusHasUpdate, StatusFetching, StatusUnavailable} Status;
    private:
        Status _status=StatusUndefined;
    public:
        static pBundle create();
        static pBundle createWithJSONRepresentation(const std::string & JSONString);
        std::string JSONRepresentation() const;
        
        const std::string & title() const{return _title;}
        const std::string & identifier() const{return _identifier;}
        const std::string & localPath() const{return _localPath;}//always tokenized!, need to process with mcbPath
        const std::string & remoteURL() const{return _remoteURL;}
        
        Status status() const{return _status;}
        float version() const{return _version;}
        long downloadTimestamp() const{return _downloadTimestamp;}
        
        //user metadata
        bool boolForKey(const std::string & key, bool defaultVal=false) const;
        void setBoolForKey(const std::string & key, bool value);
        
        int intForKey(const std::string & key, int defaultVal=0) const;
        void setIntForKey(const std::string & key, int value);
        
        long longForKey(const std::string & key, long defaultVal=0) const;
        void setLongForKey(const std::string & key, long value);

        float floatForKey(const std::string & key, float defaultVal=0.f) const;
        void setFloatForKey(const std::string & key, float value);

        std::string stringForKey(const std::string & key, const std::string & defaultVal="") const;
        void setStringForKey(const std::string & key, const std::string & value);

        void serializeUserData();
        
        virtual ~Bundle()=default;
    };
}}}
#endif /* defined(__SoundSurfer__mcbBundle__) */
