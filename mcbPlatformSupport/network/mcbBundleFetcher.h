//
//  mcbBundleFetcher.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/12/14.
//
//

#ifndef __SoundSurfer__mcbBundleFetcher__
#define __SoundSurfer__mcbBundleFetcher__

#include "mcbFactory.h"
#include "mcbHTTPRequest.h"
#include "mcbBundle.h"

namespace mcb{namespace PlatformSupport{namespace network{
    extern const std::string kFetchedBundlesPathToken;
    typedef std::vector<pBundle> pBundles;
    class BundleFetcher : public SingletonFactory<BundleFetcher>{
        friend Bundle;
        friend SingletonFactory<BundleFetcher>;
        pBundles _bundles;
        
        struct Metadata{
            cocos2d::CCDictionary * metadata=nullptr;//retained
            float version=-HUGE_VALF;
            std::string url;
            std::string downloadedMetadataPath;
            bool setMetadata(cocos2d::CCDictionary *metadata);//return true if updated, nullptr will be ignored, previous data kept
            bool updateDownloadedMetadata();
            bool hasMetadata() const{return metadata!=nullptr;}
            ~Metadata();
        }_metadata;
        
        std::string _tempPathForDownloadingAsset(const std::string & path){return path+".download";}
        
        bool _isSynchronizing=false;
        
        void _fetchMetadata();
        
        void _createBundlesFromMetadata();
        void _saveBundlesUserdata();
        void _restoreBundlesUserdata();
        
        void _fetchBundle(pBundle bundle, const std::function<void(bool success)> & completion, const std::function<void(float progress)> & progress=nullptr);
        
    public:
        //this will not have effect, once the initial connection with server is established. tokens injected onto mcbPath
        void initPreshippedDataWithPath(const std::string path);
        void updateMetadataFromServer();//this will trigger synchronize with server if successful
        bool synchronizeWithServer();//update or download bundles
        bool isSynchronizingWithServer() const;
        pBundle bundleByIdentifier(const std::string & identifier) const;
        const pBundles & bundles() const;

        //once bundle is downloaded, new tokens will be injected to mcbPath
        void downloadAndUnzipBundle(const HTTPRequest & request, const std::string & bundlesDirectory, const std::function<void(const std::string & bundlePath, bool success)> & completion, const std::string & bundleID="", const std::function<void(float progress)> & progress=nullptr);
        

    protected:
        virtual void init() override;
        BundleFetcher()=default;
        virtual ~BundleFetcher()=default;
    };
}}}

#endif /* defined(__SoundSurfer__mcbBundleFetcher__) */
