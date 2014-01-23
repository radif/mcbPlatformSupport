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
        _catalogMetadata.downloadedMetadataPath=PlatformSupport::Functions::stringByAppendingPathComponent(fetchedBundlesPath, "catalog.data");
        
        mcbLog("metadata path \"%s\"",_catalogMetadata.downloadedMetadataPath.c_str());
        
        
        _deserializeBundles();//get the previously saved bundles first
        
        _catalogMetadata.updateDownloadedMetadata();
        fetchMetadata();
                
    }
    bool BundleCatalog::fetchMetadata(const std::function<void(bool hasNewVersion, NetworkTask::Status status)> & completion){
        if (_catalogMetadata.url.empty())
            return false;
        if (_isDownloadingBundles)
            return false;
        
        _isDownloadingBundles=true;
        
        DownloadQueue::sharedInstance()->enqueueDownload(HTTPRequestGET(_catalogMetadata.url), _tempPathForDownloadingAsset(_catalogMetadata.downloadedMetadataPath), [=](DownloadTask::Status status, const HTTPResponse & response){
            if (status==DownloadTask::StatusCompleted){
                
                if(_catalogMetadata.updateDownloadedMetadata(_tempPathForDownloadingAsset(_catalogMetadata.downloadedMetadataPath))){
                    mcbLog("updated metadata to version %f!",_catalogMetadata.version);
                    cocos2d::CCNotificationCenter::sharedNotificationCenter()->postNotification(kBundlesMetadataUpdatedNotificationName.c_str());
                    if (completion)
                        completion(true, status);
                }else{
                    mcbLog("Don't need to update metadata, keeping current version: %f",_catalogMetadata.version);
                    if (completion)
                        completion(false, status);
                }
            }else{
                mcbLog("Download metadata failed, keeping current version: %f",_catalogMetadata.version);
                if (completion)
                    completion(false, status);
            }
            _isDownloadingBundles=false;
        });
        return true;
    }
    bool BundleCatalog::Metadata::updateDownloadedMetadata(const std::string & downloadedPathOrEmpty){
        cocos2d::CCDictionary * m(nullptr);
        
        //copying file if downloadedPathOrEmpty is in fact valid and has an update
        bool needsCopyFileIfValid(false);
        std::string path;
        if (downloadedPathOrEmpty.empty()) 
            path=downloadedMetadataPath;
        else{
            if (downloadedPathOrEmpty!=downloadedMetadataPath)
                needsCopyFileIfValid=true;
            
            path=downloadedPathOrEmpty;
        }
        
        if (Functions::fileExists(path))
            m=PlatformSupport::dictionaryFromPlist(path);
        
        bool retVal(setMetadata(m));
        //move file if qualified
        if (retVal && needsCopyFileIfValid)
            Functions::renameFile(downloadedPathOrEmpty, downloadedMetadataPath);
        else
            Functions::removeFile(downloadedPathOrEmpty);
        return retVal;
    }
    void BundleCatalog::initPreshippedDataWithPath(const std::string path){
        if(_catalogMetadata.updateDownloadedMetadata(path))
            fetchMetadata();
    }
    void BundleCatalog::_applyMetadataToBundles(){
        if (!_catalogMetadata.hasMetadata())
            return;
        using namespace cocos2d;
        
        cocos2d::CCArray * bundlesA((cocos2d::CCArray *)_catalogMetadata.metadata->objectForKey("bundles"));
        if (bundlesA) {
            
            pBundles newBundles;
            
            for (int i(0); i<bundlesA->count(); ++i) {
                cocos2d::CCDictionary * bundleDict((cocos2d::CCDictionary *)bundlesA->objectAtIndex(i));
                
                std::string identifier(Functions::stringForObjectKey(bundleDict, "identifier"));
                if (identifier.empty())
                    continue;
                float version(Functions::floatForObjectKey(bundleDict, "version",-HUGE_VALF));
                pBundle b(bundleByIdentifier(identifier));
                pBundle newBundle(b?Bundle::createByCopy(b):Bundle::create());
                
                auto mergeUserInfo([&](){
                    //merge content labels
                    CCArray * labelsA(dynamic_cast<CCArray *>(bundleDict->objectForKey("content_labels")));
                    if (labelsA && labelsA->count()) {
                        mcbForEachBegin(CCString *, s, labelsA)
                            newBundle->_contentLabels.emplace(s->m_sString);
                        mcbForEachEnd
                    }
                    
                    //merge user metadata
                    CCDictionary * userMetadataD(dynamic_cast<CCDictionary *>(bundleDict->objectForKey("user_metadata")));
                    if (userMetadataD) {
                        CCDictElement *e(nullptr);
                        CCDICT_FOREACH(userMetadataD, e)
                            newBundle->_userMetadata[e->getStrKey()]=((CCString *)e->getObject())->m_sString;
                    }
                });
                if (b) {
                    //exisitng bundle
                    if (newBundle->_version<version){
                        //if downloaded, then
                        if (newBundle->_status==Bundle::StatusDownloaded)
                            newBundle->_status=Bundle::StatusUpdateAvailableOnline;
                    }if (newBundle->_version==version) {
                        //update all settings
                        newBundle->_title=Functions::stringForObjectKey(bundleDict, "title");
                        newBundle->_preshipped=Functions::boolForObjectKey(bundleDict, "preshipped",newBundle->_preshipped);
                        if (newBundle->_preshipped){
                            newBundle->_status=Bundle::StatusDownloaded;
                        }
                        
                        //this is a post download case
                        if (newBundle->_status==Bundle::StatusDownloaded){
                            mergeUserInfo();
                            if (newBundle->_preshipped){
                                //local paths only applicable to preshipped and once downloaded
                                newBundle->_localPath=Functions::stringForObjectKey(bundleDict, "local_path", newBundle->_localPath);
                            }
                        }
                    }
                    
                    
                }else{
                    //new bundle
                    newBundle->_identifier=identifier;
                    newBundle->_title=Functions::stringForObjectKey(bundleDict, "title");
                    newBundle->_version=version;
                    newBundle->_preshipped=Functions::boolForObjectKey(bundleDict, "preshipped",newBundle->_preshipped);
                    if (newBundle->_preshipped){
                        newBundle->_status=Bundle::StatusDownloaded;
                        newBundle->_downloadTimestamp=time(0);
                        //local pathis only applicable to preshipped
                        newBundle->_localPath=Functions::stringForObjectKey(bundleDict, "local_path", newBundle->_localPath);
                    }else
                        newBundle->_status=Bundle::StatusAvailableOnline;
                    
                    mergeUserInfo();
                    
                }
                //always keep this current for upgrades
                newBundle->_remoteURL=Functions::stringForObjectKey(bundleDict, "remote_url", newBundle->_remoteURL);

             
                newBundles[newBundle->_identifier]=std::move(newBundle);
            }
            
            //mark the rest as deleted
            auto foundInNewBundlesL([&](const std::string & identifier){
                return newBundles.find(identifier)!=newBundles.end();
            });
            
            for (const auto & p : _bundles)
                if (!foundInNewBundlesL(p.second->_identifier))
                    _addToDeletedBundles(p.second);
            
            _bundles=std::move(newBundles);
        }
        
        _serializeBundles();
    }
    void BundleCatalog::_addToDeletedBundles(pBundle b){
        //make sure it is not in bundles
//        auto it(_bundles.find(b->_identifier));
//        if (it!=_bundles.end())
 //           _bundles.erase(it);
            
        b->_status=Bundle::StatusScheduledForDeletion;
        _deletedBundles[b->_identifier]=b;
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
            if (bundleV.IsObject() && bundleV.HasMember("identifier")) {
                retVal=Bundle::create();
                //identifier
                retVal->_identifier=bundleV["identifier"].GetString();
                
                if (bundleV.HasMember("title"))
                    retVal->_title=bundleV["title"].GetString();
                if (bundleV.HasMember("version"))
                    retVal->_version=bundleV["version"].GetDouble();
                if (bundleV.HasMember("download_timestamp"))
                    retVal->_downloadTimestamp=bundleV["download_timestamp"].GetInt();
                if (bundleV.HasMember("status"))
                    retVal->_status=statusFromString(bundleV["status"].GetString());
                if (bundleV.HasMember("local_path"))
                    retVal->_localPath=bundleV["local_path"].GetString();
                if (bundleV.HasMember("remote_url"))
                    retVal->_remoteURL=bundleV["remote_url"].GetString();
                if (bundleV.HasMember("preshipped"))
                    retVal->_preshipped=bundleV["preshipped"].GetBool();
                
                //content labels
                if (bundleV.HasMember("content_labels")){
                    const Value & contentLabelsV(bundleV["content_labels"]);
                    if (contentLabelsV.IsArray()) {
                        decltype(retVal->_contentLabels) contentLabels;
                        contentLabels.reserve(contentLabelsV.Size());
                        for (SizeType i(0); i < contentLabelsV.Size(); ++i)
                            contentLabels.emplace(contentLabelsV[i].GetString());
                        retVal->_contentLabels=std::move(contentLabels);
                    }
                }
                //user metadata
                if (bundleV.HasMember("user_metadata")){
                    const Value & userMetadataV(bundleV["user_metadata"]);
                    if (userMetadataV.IsObject()) {
                        decltype(retVal->_userMetadata) userMetadata;
                        for (auto itr(userMetadataV.MemberBegin()); itr!=userMetadataV.MemberEnd();++itr)
                            userMetadata[itr->name.GetString()]=itr->value.GetString();
                        retVal->_userMetadata=std::move(userMetadata);
                    }
                }
                
                
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
    
    void BundleCatalog::completeSynchronize(const std::function<bool()> & canDeleteBundles, const std::function<void(bool hasNewBundles)> & completion){
        auto canDeleteL([=]()->bool{
            if (!canDeleteBundles)
                return false;
            return canDeleteBundles();
        });
        if (canDeleteL())
            deleteUpdatedBundles();
        fetchMetadata([=](bool hasNewVersion, NetworkTask::Status status){
            if (canDeleteL())
                deleteUpdatedBundles();
            synchronizeAllBundlesWithServer(nullptr, [=](bool hasNewBundles, NetworkTask::Status status){
                if (canDeleteL())
                    deleteUpdatedBundles();

                if (completion)
                    completion(hasNewBundles);
            });
            
            
        });
        
    }
    bool BundleCatalog::synchronizeAllBundlesWithServer(const std::function<void(pBundle bundle, bool & stop)> & completionPerBundle, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion){
        return synchronizeBundlesWithServer(bundleIdentifiers(), completionPerBundle, completion);
    }
    bool BundleCatalog::synchronizeBundlesWithServer(const std::vector<std::string> & bundleIdentifiers, const std::function<void(pBundle bundle, bool & stop)> & completionPerBundle, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion){
        if (_isDownloadingBundles)
            return false;
        _isDownloadingBundles=true;
        bool hasNewBundles(false);
        
        //the updated or downloaded bundles will be newly created bundles
        
        {//other thread?
            
            
            //-------------------
            //each (if so):
            hasNewBundles=true;
            cocos2d::CCNotificationCenter::sharedNotificationCenter()->postNotification(kBundlesUpdatedNotificationName.c_str());
            if (completionPerBundle){
                bool shouldStop(false);
                completionPerBundle(nullptr, shouldStop);//bundle?
                if (shouldStop) {
                    //TODO: finish up
                }
            }
            
            //-------------------
            
            
            //DONE:
            _isDownloadingBundles=false;
            cocos2d::CCNotificationCenter::sharedNotificationCenter()->postNotification(kBundlesUpdatedNotificationName.c_str());
            if (completion)
                completion(hasNewBundles, NetworkTask::StatusCompleted);
        }
        
        return true;
        
    }
    
    bool BundleCatalog::deleteUpdatedBundles(){
        bool retVal(false);
        //find deleted bundles and delete from disk their content
        for (const auto & p : _deletedBundles){
            if (!p.second->_preshipped && Functions::fileExists(p.second->_localPath))
                Functions::removeFile(p.second->_localPath);
            
            //TODO: delete all unfinished downloads and un-unzipped bundles
            
            retVal=true;
        }
        
        //flush
        decltype(_deletedBundles) cleanVector;
        std::swap(_deletedBundles, cleanVector);
        
        _serializeBundles();
        
        //TODO: delete all unfinished downloads and un-unzipped bundles
        
        return retVal;
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
        return retVal;
    }
    std::vector<pBundle> BundleCatalog::localBundles() const{
        std::vector<pBundle> retVal;
        retVal.reserve(_bundles.size());
        for (const auto & p: _bundles){
            if (p.second->isLocallyAvailable())
                retVal.emplace_back(p.second);
        }
        return retVal;
    }
    std::vector<pBundle> BundleCatalog::deletedBundles() const{
        std::vector<pBundle> retVal;
        retVal.reserve(_deletedBundles.size());
        for (const auto & p: _deletedBundles)
            retVal.emplace_back(p.second);
        return retVal;
    }
    std::vector<std::string> BundleCatalog::bundleIdentifiers() const{
        std::vector<std::string> retVal;
        retVal.reserve(_bundles.size());
        for (const auto & p: _bundles)
            retVal.emplace_back(p.second->identifier());
        return retVal;
    }
    std::vector<std::string> BundleCatalog::localBundleIdentifiers() const{
        std::vector<std::string> retVal;
        retVal.reserve(_bundles.size());
        for (const auto & p: _bundles)
            if (p.second->isLocallyAvailable())
                retVal.emplace_back(p.second->identifier());
        return retVal;
    }
    std::vector<pBundle> BundleCatalog::bundlesWithContentLabel(const std::string & label){
        std::vector<pBundle> retVal;
        for (const auto & p: _bundles)
            for (const std::string & pLabel : p.second->_contentLabels)
                if (pLabel==label)
                    retVal.emplace_back(p.second);
        return retVal;
    }
    std::vector<pBundle> BundleCatalog::localBundlesWithContentLabel(const std::string & label){
        std::vector<pBundle> retVal;
        for (const auto & p: _bundles)
            if (p.second->isLocallyAvailable())
                for (const std::string & pLabel : p.second->_contentLabels)
                    if (pLabel==label)
                        retVal.emplace_back(p.second);
        return retVal;;
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