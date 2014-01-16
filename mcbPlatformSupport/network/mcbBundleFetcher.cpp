//
//  mcbBundleFetcher.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/12/14.
//
//

#include "mcbBundleFetcher.h"
#include "mcbDownloadQueue.h"
#include "mcbUnzipQueue.h"
#include "mcbPlatformSupport.h"
#include "mcbPlatformSupportFunctions.h"
#include <sys/stat.h>

namespace mcb{namespace PlatformSupport{namespace network{
    const std::string kFetchedBundlesPathToken("$(FETCHED)");

    BundleFetcher::Metadata::~Metadata(){
        CC_SAFE_RELEASE(metadata);
    }
    void BundleFetcher::init(){
        
        //generate bundle downloads path
        const std::string fetchedBundlesPath(PlatformSupport::resolvePath("$(LIBRARY)/fetched_bundles/"));
        
        //create if doesn't exist
        if(!cocos2d::CCFileUtils::sharedFileUtils()->isFileExist(fetchedBundlesPath))
            mkdir(fetchedBundlesPath.c_str(),0777);
        
        //inject token
        PlatformSupport::addTokenForPath(kFetchedBundlesPathToken, fetchedBundlesPath);
        
        //search for downloaded metadata
        _metadata.downloadedMetadataPath=PlatformSupport::Functions::stringByAppendingPathComponent(fetchedBundlesPath, "metadata.data");
        
        _metadata.updateDownloadedMetadata();
        _fetchMetadata();
        
    }
    void BundleFetcher::_fetchMetadata(){
        if (_metadata.url.empty())
            return;
        
        DownloadQueue::sharedInstance()->enqueueDownload(HTTPRequestGET(_metadata.url), _metadata.downloadedMetadataPath, [=](DownloadTask::Status status, const HTTPResponse & response){
            if (status==DownloadTask::StatusCompleted)
                if(_metadata.updateDownloadedMetadata())
                    synchronizeWithServer();
        });
    }
    bool BundleFetcher::Metadata::updateDownloadedMetadata(){
        cocos2d::CCDictionary * m(nullptr);
        if (cocos2d::CCFileUtils::sharedFileUtils()->isFileExist(downloadedMetadataPath))
            m=PlatformSupport::dictionaryFromPlist(downloadedMetadataPath);
        
        return setMetadata(m);
    }
    void BundleFetcher::initPreshippedDataWithPath(const std::string path){
        cocos2d::CCDictionary * m(nullptr);
        if (cocos2d::CCFileUtils::sharedFileUtils()->isFileExist(path))
            m=PlatformSupport::dictionaryFromPlist(path);
        
        if(_metadata.setMetadata(m)){
            _fetchMetadata();
            _createBundlesFromMetadata();
        }
    }
    void BundleFetcher::_createBundlesFromMetadata(){
        if (!_metadata.hasMetadata())
            return;
        
        _saveBundlesUserdata();
        //create new bundles
        pBundles newBundles;
        
        cocos2d::CCArray * bundlesA((cocos2d::CCArray *)_metadata.metadata->objectForKey("bundles"));
        if (bundlesA) {
         
            newBundles.reserve(bundlesA->count());
            for (int i(0); i<bundlesA->count(); ++i) {
                cocos2d::CCDictionary * bundleDict((cocos2d::CCDictionary *)bundlesA->objectAtIndex(i));
                pBundle b(Bundle::create());
                b->_identifier=PlatformSupport::Functions::stringForObjectKey(bundleDict, "identifier");
                b->_localPath=PlatformSupport::Functions::stringByAppendingPathComponent(kFetchedBundlesPathToken, b->_identifier);
                b->_title=PlatformSupport::Functions::stringForObjectKey(bundleDict, "title");
                
                //??b->_version=PlatformSupport::Functions::floatForObjectKey(bundleDict, "version");
                
                newBundles.emplace_back(b);
            }
        }
        _bundles=std::move(newBundles);
        _restoreBundlesUserdata();
    }
    void BundleFetcher::_saveBundlesUserdata(){
        
    }
    void BundleFetcher::_restoreBundlesUserdata(){
        
    }

    bool BundleFetcher::isSynchronizingWithServer() const{
        return _isSynchronizing;
    }
    void BundleFetcher::updateMetadataFromServer(){
        _fetchMetadata();
    }
    bool BundleFetcher::synchronizeWithServer(){
        if (_isSynchronizing)
            return false;
        _isSynchronizing=true;
        
        
        {//other thread?
            
        _isSynchronizing=false;
        }
        
        return true;
        
    }
    
    bool BundleFetcher::Metadata::setMetadata(cocos2d::CCDictionary *m){
        if (!m)
            return false;
        
        float newVersion(PlatformSupport::Functions::floatForObjectKey(m, "version",version));
        if (newVersion>version) {
            CC_SAFE_RELEASE(metadata);
            metadata=m;
            CC_SAFE_RETAIN(metadata);
            
            //update version and URL
            version=newVersion;
            url=PlatformSupport::Functions::stringForObjectKey(metadata, "url",url);
            return true;
        }
        return false;
    }
    
    pBundle BundleFetcher::bundleByIdentifier(const std::string & identifier) const{
        for (const pBundle & b :_bundles)
            if (b->identifier()==identifier)
                return b;
        return nullptr;
    }
    const pBundles & BundleFetcher::bundles() const{
        return _bundles;
    }
    
    void BundleFetcher::_fetchBundle(pBundle bundle, const std::function<void(bool success)> & completion, const std::function<void(float progress)> & progress){
        
        static const float kDownloadToUnpackProgressRatio(.7f);
        
        auto setProgressL([=](float p){
            if (progress)
                progress(p);
        });
        
        
        
        std::string bundleHash(bundle->identifier());
        std::string bundlePath(bundle->localPath());
        std::string bundleZipPath(bundlePath+".zip");
        std::string bundleUnZipPath(bundlePath+"/");
        
        auto callCompletionL([=](bool success){
            if (completion)
                completion(success);
        });
        
        auto cleanupL([=](bool both){
            remove(bundleZipPath.c_str());
            if (both)
                remove(bundleUnZipPath.c_str());
        });
        
        cleanupL(true);
        DownloadQueue::sharedInstance()->enqueueDownload(HTTPRequestGET(bundle->remoteURL()), bundleZipPath, [=](NetworkTask::Status status, const HTTPResponse & response){
            
            if (status==NetworkTask::StatusCompleted) {
                UnzipQueue::sharedInstance()->enqueueUnzip(bundleZipPath, bundleUnZipPath, [=](UnzipTask::Status status){
                    
                    if (status==UnzipTask::StatusCompleted) {
                        cleanupL(false);
                        setProgressL(1.f);
                        callCompletionL(true);
                    }else{
                        callCompletionL(false);
                        cleanupL(true);
                    }
                    
                },[=](float prog){//progress
                    setProgressL(kDownloadToUnpackProgressRatio + (1.f-kDownloadToUnpackProgressRatio) * prog);
                });
            }else{
                callCompletionL(false);
                cleanupL(false);
            }
            
        },[=](float prog){//progress
            setProgressL(kDownloadToUnpackProgressRatio * prog);
        });
        
    }

    
    void BundleFetcher::downloadAndUnzipBundle(const HTTPRequest & request, const std::string & bundlesDirectory, const std::function<void(const std::string & bundlePath, bool success)> & completion, const std::string & bundleID, const std::function<void(float progress)> & progress){
        
        static const float kDownloadToUnpackProgressRatio(.7f);
        
        auto setProgressL([=](float p){
            if (progress)
                progress(p);
        });
        
        
        
        std::string bundleHash(bundleID.empty()?Functions::generateRandomAlphanumericString():bundleID);
        std::string bundlePath(Functions::stringByAppendingPathComponent(bundlesDirectory, bundleHash));
        std::string bundleZipPath(bundlePath+".zip");
        std::string bundleUnZipPath(bundlePath+"/");
        
        auto callCompletionL([=](bool success){
            if (completion)
                completion(bundleUnZipPath, success);
        });
        
        auto cleanupL([=](bool both){
            remove(bundleZipPath.c_str());
            if (both)
                remove(bundleUnZipPath.c_str());
        });
        
        cleanupL(true);
        DownloadQueue::sharedInstance()->enqueueDownload(request, bundleZipPath, [=](NetworkTask::Status status, const HTTPResponse & response){
            
            if (status==NetworkTask::StatusCompleted) {
                UnzipQueue::sharedInstance()->enqueueUnzip(bundleZipPath, bundleUnZipPath, [=](UnzipTask::Status status){
                    
                    if (status==UnzipTask::StatusCompleted) {
                        cleanupL(false);
                        setProgressL(1.f);
                        callCompletionL(true);
                    }else{
                        callCompletionL(false);
                        cleanupL(true);
                    }
                    
                },[=](float prog){//progress
                    setProgressL(kDownloadToUnpackProgressRatio + (1.f-kDownloadToUnpackProgressRatio) * prog);
                });
            }else{
                callCompletionL(false);
                cleanupL(false);
            }
            
        },[=](float prog){//progress
            setProgressL(kDownloadToUnpackProgressRatio * prog);
        });
        
    }
    
}}}