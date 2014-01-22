//
//  mcbBundleCatalog.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/12/14.
//
//

#ifndef __SoundSurfer__mcbBundleCatalog__
#define __SoundSurfer__mcbBundleCatalog__

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
        pBundles _bundles;
        pBundles _deletedBundles;

        std::string _jsonPath;
        
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
        
        void _fetchBundle(pBundle bundle, const std::function<void(bool success)> & completion, const std::function<void(float progress)> & progress=nullptr);
        
        //deprecate?
        //once bundle is downloaded, new tokens will be injected to mcbPath
        void downloadAndUnzipBundle(const HTTPRequest & request, const std::string & bundlesDirectory, const std::function<void(const std::string & bundlePath, bool success)> & completion, const std::string & bundleID="", const std::function<void(float progress)> & progress=nullptr);

    public:
        
        //this will not have effect, once the initial connection with server is established. tokens injected onto mcbPath
        void initPreshippedDataWithPath(const std::string path);
        bool fetchMetadata(const std::function<void(bool hasNewVersion, NetworkTask::Status status)> & completion=nullptr);
        
        
        //the updated or downloaded bundles will be newly created bundles, so you can continue using the old one until you delete their assets by calling deleteUpdatedBundles();
        bool synchronizeBundlesWithServer(const std::vector<std::string> & bundleIdentifiers, const std::function<void(pBundle bundle)> & completionPerBundle=nullptr, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion=nullptr);//update or download bundles
        bool synchronizeAllBundlesWithServer(const std::function<void(pBundle bundle)> & completionPerBundle=nullptr, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion=nullptr);//update or download bundles
        
        //will fetch catalog metadata, then update/download all bundles, delete from disk, if allowed by first argument's return
        void completeSynchronize(const std::function<bool()> & canDeleteBundles=nullptr, const std::function<void()> & completion=nullptr);
        
        bool deleteUpdatedBundles();
        void serializeBundles(){_serializeBundles();}
        bool isDownloadingBundles() const;
        
        //get bundles
        //bundles, once given will not be modified by sdk. SDK will create new bundles if updated from server. However, the user metadata will be merged if saved
        pBundle bundleByIdentifier(const std::string & identifier) const;
        std::vector<pBundle> bundles() const;
        std::vector<std::string> bundleIdentifiers() const;
        std::vector<pBundle> bundlesWithContentLabel(const std::string & label);
        
        //already updated or deleted bundles, that were not physically deleted from disk
        std::vector<pBundle> deletedBundles() const;
        
    protected:
        virtual void init() override;
        BundleCatalog()=default;
        virtual ~BundleCatalog()=default;
    };
}}}

#endif /* defined(__SoundSurfer__mcbBundleCatalog__) */
