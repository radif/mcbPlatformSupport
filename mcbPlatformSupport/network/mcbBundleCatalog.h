//
//  mcbBundleCatalog.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/12/14.
//
//

#ifndef __mcb__network__mcbBundleCatalog__
#define __mcb__network__mcbBundleCatalog__

#include "mcbFactory.h"
#include "mcbHTTPRequest.h"
#include "mcbBundle.h"
#include "mcbNetworkTask.h"

namespace mcb{namespace PlatformSupport{namespace network{
    extern const std::string kFetchedBundlesPathToken;
    
    extern const std::string kBundlesUpdatedNotificationName;
    extern const std::string kBundlesMetadataUpdatedNotificationName;
    
    typedef std::map<std::string, pBundle> pBundles;
    
    class BundleCatalog : public SingletonFactory<BundleCatalog>{
        friend Bundle;
        friend SingletonFactory<BundleCatalog>;
        
        const bool kUsesBase64Serialization=true;
        
        pBundles _bundles;
        
        //has to be different map as it may have the identifiers of the updated ones
        pBundles _deletedBundles;

        std::string _jsonPath;
        std::string _fetchedPath;
        
        struct Metadata{
            cocos2d::CCDictionary * metadata=nullptr;//retained
            float version=-HUGE_VALF;
            std::string url;
            std::string downloadedMetadataPath;
            bool setMetadata(cocos2d::CCDictionary *metadata);//return true if updated, nullptr will be ignored, previous data kept
            bool updateDownloadedMetadata(const std::string & downloadedPathOrEmpty="");
            bool hasMetadata() const{return metadata!=nullptr;}
            ~Metadata();
        }_catalogMetadata;
        
        std::string _tempPathForDownloadingAsset(const std::string & path){return path+".download";}
        
        bool _isDownloadingBundles=false;
        
        void _addToDeletedBundles(pBundle b);
        void _serializeBundles();
        void _deserializeBundles();
        void _applyMetadataToBundles();
        
        std::string _pathForBundleZipfile(const pBundle & b) const;
        std::string _pathForBundleStorageDirectory(const pBundle & b, bool tokenized=false) const;
    public:
        
        //this will not have effect, once the initial connection with server is established. tokens injected onto mcbPath
        void initPreshippedDataWithPath(const std::string path);
        bool fetchMetadata(const std::function<void(bool hasNewVersion, NetworkTask::Status status)> & completion=nullptr);
        
        
        //the updated or downloaded bundles will be newly created bundles, so you can continue using the old one until you delete their assets by calling deleteUpdatedBundles();
        bool synchronizeBundlesWithServer(const std::vector<std::string> & bundleIdentifiers, const std::function<void(pBundle bundle, bool success)> & completionPerBundle=nullptr, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion=nullptr);//update or download bundles
        bool synchronizeAllBundlesWithServer(const std::function<void(pBundle bundle, bool success)> & completionPerBundle=nullptr, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion=nullptr);//update or download bundles
        
        //will fetch catalog metadata, then update/download all bundles, delete from disk, if allowed by first argument's return
        void completeSynchronize(const std::function<bool()> & canDeleteBundles=nullptr, const std::function<void(bool hasNewBundles)> & completion=nullptr);
        
        bool hasDeletedBundles() const{return !_deletedBundles.empty();}
        bool deleteUpdatedBundles();
        void serializeBundles(){_serializeBundles();}
        bool isDownloadingBundles() const;
        
        //get bundles
        //bundles, once given will not be modified by sdk. SDK will create new bundles if updated from server. However, the user metadata will be merged if saved
        pBundle bundleByIdentifier(const std::string & identifier) const;
        
        //all
        std::vector<pBundle> bundles() const;
        //downloaded, updated, preshipped
        std::vector<pBundle> localBundles() const;
        
        //all
        std::vector<std::string> bundleIdentifiers() const;
        //downloaded, updated, preshipped
        std::vector<std::string> localBundleIdentifiers() const;
        
        //all
        std::vector<pBundle> bundlesWithContentLabel(const std::string & label);
        //downloaded, updated, preshipped
        std::vector<pBundle> localBundlesWithContentLabel(const std::string & label);
        
        //already updated or deleted bundles, that were not physically deleted from disk
        std::vector<pBundle> deletedBundles() const;
        
    protected:
        virtual void init() override;
        BundleCatalog()=default;
        virtual ~BundleCatalog()=default;
    };
}}}

#endif /* defined(__mcb__network__mcbBundleCatalog__) */
