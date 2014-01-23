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
#include <vector>
#include <math.h>
#include <unordered_set>

namespace mcb{namespace PlatformSupport{namespace network{
    class BundleCatalog;
    class Bundle;
    typedef std::shared_ptr<Bundle> pBundle;
    class Bundle{
        friend BundleCatalog;
        float _version=-HUGE_VALF;
        float _nextAvailableVersion=-HUGE_VALF;
        const long long kTimestampUndefined=-1;
        long long _downloadTimestamp=kTimestampUndefined;
        std::string _identifier;
        std::string _localPath;//tokenized, need to process with mcbPath
        std::string _remoteURL;
        std::string _title;
        bool _preshipped=false;
        std::map<std::string, std::string> _userMetadata;
        std::unordered_set<std::string> _contentLabels;//these are coming from the server
        
        Bundle()=default;//avoiding make_shared in sake of hiding constructor to allow the creation of object through create func
        static pBundle create();//only bundle catalog can create bundles!
        static pBundle createByCopy(const pBundle & existing);//only bundle catalog can create bundles!
    public:
        typedef enum {StatusUndefined, StatusAvailableOnline, StatusDownloaded, StatusUpdateAvailableOnline, StatusScheduledForDeletion} Status;
    private:
        Status _status=StatusUndefined;
    public:
        bool isLocallyAvailable() const{return _status==StatusDownloaded || _status==StatusUpdateAvailableOnline;}
        bool needsFetch() const{return _status==StatusAvailableOnline || _status==StatusUpdateAvailableOnline;}
        std::string suggestedStorageName() const;
        bool isPreshipped() const{return _preshipped;}
        const std::string & title() const{return _title;}
        const std::string & identifier() const{return _identifier;}
        const std::string & localPath() const{return _localPath;}//always tokenized!, need to process with mcbPath
        const std::string & remoteURL() const{return _remoteURL;}
        const std::unordered_set<std::string> & contentLabels() const{return _contentLabels;}

        
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

        //this will merge with the official bundle's user metadata, if disjointed. If catalog no longer has it, the data won't be saved
        void serializeUserData();
        
        //does catalog have a different instance of the same bundle? If so, it may be modified, but it is good to keep the current reference to continue accessing it's assets. Disjointed bundles have to be deleted once they are deleted by catalog due to upgrade or deletion. look for StatusScheduledForDeletion
        bool isDisjointedFromCatalog() const;
        
        bool hasLocalAssets() const;
        
        virtual ~Bundle()=default;
    };
}}}
#endif /* defined(__SoundSurfer__mcbBundle__) */
