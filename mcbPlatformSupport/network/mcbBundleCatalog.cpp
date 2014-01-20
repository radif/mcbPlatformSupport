//
//  mcbBundleCatalog.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/12/14.
//
//

#include "mcbBundleCatalog.h"
#include "mcbDownloadQueue.h"
#include "mcbUnzipQueue.h"
#include "mcbPlatformSupport.h"
#include "mcbPlatformSupportFunctions.h"

//json
#include "rapidjson.h"
#include "prettywriter.h"	// for stringify JSON
#include "filestream.h"	// wrapper of C stream for prettywriter as output
#include "document.h"

namespace mcb{namespace PlatformSupport{namespace network{
    const std::string kFetchedBundlesPathToken("$(FETCHED)");
    const std::string kBundlesUpdatedNotificationName("kBundlesUpdatedNotificationName");
    const std::string kBundlesMetadataUpdatedNotificationName("kBundlesMetadataUpdatedNotificationName");
    
    //serialization
    static const std::string kLocalStorageFilePath("$(LIBRARY)/fetched_bundles/");
    static const std::string kLocalStorageJSONName("bundles.data");
    
    static const std::map<Bundle::Status, std::string> kStatusEnumMap{
        {Bundle::StatusUndefined, "StatusUndefined"},
        {Bundle::StatusAvailableOnline, "StatusAvailableOnline"},
        {Bundle::StatusDownloaded, "StatusDownloaded"},
        {Bundle::StatusIsDownloading, "StatusIsDownloading"},
        {Bundle::StatusUpdateAvailableOnline, "StatusUpdateAvailableOnline"},
        {Bundle::StatusScheduledForDeletion, "StatusScheduledForDeletion"}
    };
    
    static const std::string & stringFromStatus(Bundle::Status status){
        auto it(kStatusEnumMap.find(status));
        if (it!=kStatusEnumMap.end())
            return it->second;
        return (*kStatusEnumMap.cbegin()).second;
    }
    static Bundle::Status statusFromString(const std::string & statusString){
        for (const auto & p: kStatusEnumMap)
            if (p.second==statusString)
                return p.first;
        return Bundle::StatusUndefined;
    }
    
    BundleCatalog::Metadata::~Metadata(){
        CC_SAFE_RELEASE(metadata);
    }
    void BundleCatalog::init(){
        
        _logPrefix="\n\n-----BundleCatalog------\n";
        _logSuffix="\n------------------------\n\n";
        
        //generate bundle downloads path
        const std::string fetchedBundlesPath(resolvePath(kLocalStorageFilePath));
        
        _jsonPath=Functions::stringByAppendingPathComponent(resolvePath(kLocalStorageFilePath), kLocalStorageJSONName);
        
        //create if doesn't exist
        if(!Functions::fileExists(fetchedBundlesPath))
            Functions::createDirectory(fetchedBundlesPath);
        
        //inject token
        PlatformSupport::addTokenForPath(kFetchedBundlesPathToken, fetchedBundlesPath);
        
        //search for downloaded metadata
        _metadata.downloadedMetadataPath=PlatformSupport::Functions::stringByAppendingPathComponent(fetchedBundlesPath, "catalog.data");
        
        mcbLog("metadata path \"%s\"",_metadata.downloadedMetadataPath.c_str());
        
        
        _deserializeBundles();//get the previously saved bundles first
        
        _metadata.updateDownloadedMetadata();
        fetchMetadata();
                
    }
    bool BundleCatalog::fetchMetadata(const std::function<void(bool hasNewVersion, NetworkTask::Status status)> & completion){
        if (_metadata.url.empty())
            return false;
        if (_isDownloadingBundles)
            return false;
        
        _isDownloadingBundles=true;
        
        DownloadQueue::sharedInstance()->enqueueDownload(HTTPRequestGET(_metadata.url), _tempPathForDownloadingAsset(_metadata.downloadedMetadataPath), [=](DownloadTask::Status status, const HTTPResponse & response){
            if (status==DownloadTask::StatusCompleted){
                Functions::removeFile(_metadata.downloadedMetadataPath);
                Functions::renameFile(_tempPathForDownloadingAsset(_metadata.downloadedMetadataPath), _metadata.downloadedMetadataPath);
                if(_metadata.updateDownloadedMetadata()){
                    mcbLog("updated metadata to version %f!",_metadata.version);
                    cocos2d::CCNotificationCenter::sharedNotificationCenter()->postNotification(kBundlesMetadataUpdatedNotificationName.c_str());
                    if (completion)
                        completion(true, status);
                }else{
                    mcbLog("Don't need to update metadata, keeping current version: %f",_metadata.version);
                    if (completion)
                        completion(false, status);
                }
            }else{
                mcbLog("Download metadata failed, keeping current version: %f",_metadata.version);
                if (completion)
                    completion(false, status);
            }
            _isDownloadingBundles=false;
        });
        return true;
    }
    bool BundleCatalog::Metadata::updateDownloadedMetadata(){
        cocos2d::CCDictionary * m(nullptr);
        if (Functions::fileExists(downloadedMetadataPath))
            m=PlatformSupport::dictionaryFromPlist(downloadedMetadataPath);
        
        return setMetadata(m);
    }
    void BundleCatalog::initPreshippedDataWithPath(const std::string path){
        cocos2d::CCDictionary * m(nullptr);
        if (Functions::fileExists(path))
            m=PlatformSupport::dictionaryFromPlist(path);
        
        if(_metadata.setMetadata(m)){
            fetchMetadata();
        }
    }
    void BundleCatalog::_applyMetadataToBundles(){
        if (!_metadata.hasMetadata())
            return;
        
        _serializeBundles();
        //TODO: remove create new bundles
        pBundles newBundles;
        
        cocos2d::CCArray * bundlesA((cocos2d::CCArray *)_metadata.metadata->objectForKey("bundles"));
        if (bundlesA) {
         
            //newBundles.reserve(bundlesA->count());
            for (int i(0); i<bundlesA->count(); ++i) {
                cocos2d::CCDictionary * bundleDict((cocos2d::CCDictionary *)bundlesA->objectAtIndex(i));
                pBundle b(Bundle::create());
                b->_identifier=PlatformSupport::Functions::stringForObjectKey(bundleDict, "identifier");
                b->_localPath=PlatformSupport::Functions::stringByAppendingPathComponent(kFetchedBundlesPathToken, b->_identifier);
                b->_title=PlatformSupport::Functions::stringForObjectKey(bundleDict, "title");
                
                //??b->_version=PlatformSupport::Functions::floatForObjectKey(bundleDict, "version");
                
                newBundles[b->_identifier]=b;
            }
        }
        _bundles=std::move(newBundles);
    }
    void BundleCatalog::_serializeBundles(){
        const std::string kJsonPath(_jsonPath);
        const std::string kTempJsonPath(_jsonPath+"_temp");

        using namespace rapidjson;

        if (Functions::fileExists(kTempJsonPath))
            Functions::removeFile(kTempJsonPath);
        
        
        FILE *f(fopen(kTempJsonPath.c_str(), "wb"));
        if (f){
            FileStream s(f);
            PrettyWriter<FileStream> writer(s);
            
            auto writeStringL([&](const std::string & string){writer.String(string.c_str(), string.size());});
            
            auto serializeBundleL([&](const pBundle & b){
                //open
                writer.StartObject();
                
                //identifier
                writeStringL("identifier");
                writeStringL(b->_identifier);
                
                //title
                writeStringL("title");
                writeStringL(b->_title);
                
                //version
                writeStringL("version");
                writer.Double(b->_version);
                
                //download timestamp
                writeStringL("download_timestamp");
                writer.Int(b->_downloadTimestamp);
                
                //status
                writeStringL("status");
                writeStringL(stringFromStatus(b->_status));
                
                //localPath
                writeStringL("local_path");
                writeStringL(b->_localPath);
                
                //remoteURL
                writeStringL("remote_url");
                writeStringL(b->_remoteURL);

                //preshipped
                writeStringL("preshipped");
                writer.Bool(b->_preshipped);
                
                //content labels
                writeStringL("content_labels");
                writer.StartArray();
                for (const std::string & contentLabel : b->_contentLabels)
                    writeStringL(contentLabel);
                writer.EndArray();
                
                //user metadata
                writeStringL("user_metadata");
                writer.StartObject();
                for (const auto & p :b->_userMetadata) {
                    writeStringL(p.first);
                    writeStringL(p.second);
                }
                writer.EndObject();
                
                
                //close
                writer.EndObject();
                
            });
            
            writer.StartObject();

            writeStringL("Bundles");
            writer.StartArray();
            //put bundles here:
            for (const auto & p: _bundles)
                serializeBundleL(p.second);
            writer.EndArray();
            
            
            writeStringL("Deleted");
            writer.StartArray();
            //put deleted here:
            for (const auto & p: _deletedBundles)
                serializeBundleL(p.second);
            writer.EndArray();

            writer.EndObject();
            
            fclose(f);
            
            Functions::renameFile(kTempJsonPath, kJsonPath);
        }
        
    }
    void BundleCatalog::_deserializeBundles(){
        const std::string kJsonPath(_jsonPath);
        using namespace rapidjson;

        //do we clear all bundles regardless?
        
        //bail out if none detected, starting with no bundles, letting restore from metadata create bundles
        if (!Functions::fileExists(kJsonPath)){
            mcbLog("saved bundles not found");
            return;
        }
        
        
        std::string jsonString(cocos2d::CCString::createWithContentsOfFile(kJsonPath.c_str())->m_sString);
        
        Document document;
        
        //checking for errors
        if (document.Parse<0>(jsonString.c_str()).HasParseError()){
            mcbLog("saved bundles has errors, ignoring");
            return;
        }
        
        if (!document.IsObject()) {
            mcbLog("malformed json, ignoring");
            return;
        }
        
        
        //we have a valid json, let's parse it:
        
        auto parseBundleL([&](const Value& bundleV)->pBundle{
            pBundle retVal(nullptr);
            if (bundleV.IsObject()) {
             
                mcbLog("!!");
            }
            return retVal;
        });
        
        auto parseBundlesL([&](const std::string & key)->pBundles{
            pBundles retVal;
            if (document.HasMember(key.c_str())) {
                const Value& bundlesV(document[key.c_str()]);
                if (bundlesV.IsArray())
                    for (SizeType i(0); i < bundlesV.Size(); ++i){
                        pBundle b(parseBundleL(bundlesV[i]));
                        if (b)
                            retVal[b->_identifier]=std::move(b);
                    }
                
            }
            return retVal;
        });
        
        //grand scheme of things: clearing bundles and assigning new ones
        _bundles=parseBundlesL("Bundles");
        _deletedBundles=parseBundlesL("Deleted");
    }

    bool BundleCatalog::isDownloadingBundles() const{
        return _isDownloadingBundles;
    }
    bool BundleCatalog::synchronizeAllBundlesWithServer(const std::function<void(pBundle bundle)> & completionPerBundle, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion){
        return synchronizeBundlesWithServer(bundleIdentifiers(), completionPerBundle, completion);
    }
    bool BundleCatalog::synchronizeBundlesWithServer(const std::vector<std::string> & bundleIdentifiers, const std::function<void(pBundle bundle)> & completionPerBundle, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion){
        if (_isDownloadingBundles)
            return false;
        _isDownloadingBundles=true;
        bool hasNewBundles(false);
        
        {//other thread?
            
            
            //-------------------
            //each (if so):
            hasNewBundles=true;
            cocos2d::CCNotificationCenter::sharedNotificationCenter()->postNotification(kBundlesUpdatedNotificationName.c_str());
            if (completionPerBundle)
                completionPerBundle(nullptr);//bundle?
            
            //-------------------
            
            
            //DONE:
            _isDownloadingBundles=false;
            cocos2d::CCNotificationCenter::sharedNotificationCenter()->postNotification(kBundlesUpdatedNotificationName.c_str());
            if (completion)
                completion(hasNewBundles, NetworkTask::StatusCompleted);
        }
        
        return true;
        
    }
    
    bool BundleCatalog::updatePendingBundles(){
        //TODO: swap records for bundles to point to newly fetched bundles
        //TODO: add to deleted bundles
        return false;
    }
    bool BundleCatalog::deleteUpdatedBundles(){
        //TODO: find deleted bundles and delete from disk their content
        //TODO: delete all unfinished downloads and un-unzipped bundles
        return false;
    }
    
    bool BundleCatalog::Metadata::setMetadata(cocos2d::CCDictionary *m){
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
            BundleCatalog::sharedInstance()->_applyMetadataToBundles();
            return true;
        }
        return false;
    }
    
    pBundle BundleCatalog::bundleByIdentifier(const std::string & identifier) const{
        auto it(_bundles.find(identifier));
        if (it!=_bundles.end())
            return it->second;
        return nullptr;
    }
    std::vector<pBundle> BundleCatalog::bundles() const{
        std::vector<pBundle> retVal;
        retVal.reserve(_bundles.size());
        for (const auto & p: _bundles)
            retVal.emplace_back(p.second);
        return retVal;;
    }
    std::vector<std::string> BundleCatalog::bundleIdentifiers() const{
        std::vector<std::string> retVal;
        retVal.reserve(_bundles.size());
        for (const auto & p: _bundles)
            retVal.emplace_back(p.second->identifier());
        return retVal;
    }
    
    void BundleCatalog::_fetchBundle(pBundle bundle, const std::function<void(bool success)> & completion, const std::function<void(float progress)> & progress){
        
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

    //deprecate?
    void BundleCatalog::downloadAndUnzipBundle(const HTTPRequest & request, const std::string & bundlesDirectory, const std::function<void(const std::string & bundlePath, bool success)> & completion, const std::string & bundleID, const std::function<void(float progress)> & progress){
        
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