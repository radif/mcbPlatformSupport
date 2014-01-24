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
#include "mcbMainThreadCaller.h"

//json
#include "rapidjson.h"
#include "prettywriter.h"	// for stringify JSON
#include "stringbuffer.h"
#include "mcbBase64.h"
#include <fstream>
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
        
        _fetchedPath=resolvePath(kLocalStorageFilePath);
        _jsonPath=Functions::stringByAppendingPathComponent(_fetchedPath, kLocalStorageJSONName);
        
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

        //this calss doesn't fetch automatically
        //fetchMetadata();
                
    }
    bool BundleCatalog::fetchMetadata(const std::function<void(bool hasNewVersion, NetworkTask::Status status)> & completion){
        if (_catalogMetadata.url.empty())
            return false;
        if (_isDownloadingBundles)
            return false;
        
        _isDownloadingBundles=true;
        
        DownloadQueue::sharedInstance()->enqueueDownload(HTTPRequestGET(_catalogMetadata.url), _tempPathForDownloadingAsset(_catalogMetadata.downloadedMetadataPath), [=](DownloadTask::Status status, const HTTPResponse & response){
            _isDownloadingBundles=false;
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
            Functions::safeRemoveFileOrDirectoryAtPath(downloadedPathOrEmpty);
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
                        
                        newBundle->_nextAvailableVersion=version;
                        
                    }else if (newBundle->_version==version) {
                        //update all settings
                        newBundle->_nextAvailableVersion=version;
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
                    newBundle->_nextAvailableVersion=version;
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
        b->_status=Bundle::StatusScheduledForDeletion;
        _deletedBundles[b->_identifier]=b;
    }
    void BundleCatalog::_serializeBundles(){
        const std::string kJsonPath(_jsonPath);
        const std::string kTempJsonPath(_jsonPath+"_temp");

        using namespace rapidjson;

        Functions::safeRemoveFileOrDirectoryAtPath(kTempJsonPath);

       
            
        StringBuffer s;
        PrettyWriter<StringBuffer> writer(s);
        
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
            
            //next version
            writeStringL("nextAvailableVersion");
            writer.Double(b->_nextAvailableVersion);
            
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
        
        
        std::ofstream out(kTempJsonPath);
        out << (kUsesBase64Serialization?crypto::base64Encode(s.GetString()):s.GetString());
        out.close();
        
        
        Functions::renameFile(kTempJsonPath, kJsonPath);
        
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
        if (kUsesBase64Serialization)
            jsonString=crypto::base64DecodeS(jsonString);
        
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
                if (bundleV.HasMember("nextAvailableVersion"))
                    retVal->_nextAvailableVersion=bundleV["nextAvailableVersion"].GetDouble();
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
    bool BundleCatalog::synchronizeAllBundlesWithServer(const std::function<void(pBundle bundle, bool success)> & completionPerBundle, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion){
        return synchronizeBundlesWithServer(bundleIdentifiers(), completionPerBundle, completion);
    }
    bool BundleCatalog::synchronizeBundlesWithServer(const std::vector<std::string> & bundleIdentifiers, const std::function<void(pBundle bundle, bool success)> & completionPerBundle, const std::function<void(bool hasNewBundles, NetworkTask::Status status)> & completion){
        if (_isDownloadingBundles)
            return false;
        
        //the updated or downloaded bundles will be newly created bundles
        std::vector<pBundle> bundlesNeedUpdating;
        for (const std::string & identifier : bundleIdentifiers) {
            pBundle b(bundleByIdentifier(identifier));
            if (!b)
                continue;
            
            if (b->needsFetch())
                bundlesNeedUpdating.emplace_back(std::move(b));
        }

        
        if(bundlesNeedUpdating.empty()){
            call_on_main_thread([=](){
                if (completion)
                    completion(false, NetworkTask::StatusUndef);
            });
            return false;
        }
        
        _isDownloadingBundles=true;
        
        bool hasNewBundles(false);
        
        auto completionPL(std::make_shared<std::function<void(pBundle b, bool success)>>([=](pBundle b, bool success) mutable{
            
            //erase from the list
            auto it(std::find(bundlesNeedUpdating.begin(), bundlesNeedUpdating.end(), b));
            if (it!=bundlesNeedUpdating.end())
                bundlesNeedUpdating.erase(it);
            
            //----per bundle
            if (success) {
                hasNewBundles=true;
                mcbLog("downloaded bundle: %s, v%f",b->_identifier.c_str(), b->_nextAvailableVersion);
                //create a new bundle for the new version
                _addToDeletedBundles(b);
                b=Bundle::createByCopy(b);
                b->_version=b->_nextAvailableVersion;
                b->_status=Bundle::StatusDownloaded;
                b->_localPath=_pathForBundleStorageDirectory(b, true);
                if (b->_downloadTimestamp==b->kTimestampUndefined)
                    b->_downloadTimestamp=time(0);
                _bundles[b->_identifier]=b;
                
                _serializeBundles();
                cocos2d::CCNotificationCenter::sharedNotificationCenter()->postNotification(kBundlesUpdatedNotificationName.c_str());
                
            }
            if (completionPerBundle)
                completionPerBundle(b, success);
            
            
            
            
            
            //----Done:
            if (bundlesNeedUpdating.empty()) {
                _isDownloadingBundles=false;
                mcbLog("download bundles finished; has new bundles:%s", hasNewBundles?"true":"false");
                if (completion)
                    completion(hasNewBundles, NetworkTask::StatusCompleted);
            }
        }));
        
        auto callCompletionL([=](const pBundle & b, bool success){
            (*completionPL)(b, success);
        });
        
        for (pBundle b :bundlesNeedUpdating) {
            
            std::string bundleZipPath(_pathForBundleZipfile(b));
            std::string bundleUnZipPath(_pathForBundleStorageDirectory(b));
            std::string remoteURL(b->remoteURL());
            
            Functions::safeRemoveFileOrDirectoryAtPath(bundleZipPath);
            Functions::safeRemoveFileOrDirectoryAtPath(bundleUnZipPath);
            
            DownloadQueue::sharedInstance()->enqueueDownload(HTTPRequestGET(remoteURL), bundleZipPath, [=](NetworkTask::Status status, const HTTPResponse & response){
                if (status==NetworkTask::StatusCompleted) {
                    
                    Functions::createDirectory(bundleUnZipPath);
                    
                    UnzipQueue::sharedInstance()->enqueueUnzip(bundleZipPath, bundleUnZipPath, [=](UnzipTask::Status status){
                        Functions::safeRemoveFileOrDirectoryAtPath(bundleZipPath);
                        if (status==UnzipTask::StatusCompleted) {
                            callCompletionL(b, true);
                        }else{
                            Functions::safeRemoveFileOrDirectoryAtPath(bundleUnZipPath);
                            callCompletionL(b, false);
                        }
                    });
                }else{
                    Functions::safeRemoveFileOrDirectoryAtPath(bundleZipPath);
                    Functions::safeRemoveFileOrDirectoryAtPath(bundleUnZipPath);
                    callCompletionL(b, false);
                }
                
            });
        }
        
       
        return true;
    }
    
    bool BundleCatalog::deleteUpdatedBundles(){
        bool retVal(false);
        //find deleted bundles and delete from disk their content
        
        for (const auto & p : _deletedBundles){
            if (!p.second->_preshipped)
                Functions::safeRemoveFileOrDirectoryAtPath(resolvePath(p.second->_localPath));
            
            //deleting the unfinished downloads if any
            Functions::safeRemoveFileOrDirectoryAtPath(_pathForBundleZipfile(p.second));
            Functions::safeRemoveFileOrDirectoryAtPath(_pathForBundleStorageDirectory(p.second));
            
            retVal=true;
        }
        
        //flush
        decltype(_deletedBundles) cleanVector;
        std::swap(_deletedBundles, cleanVector);
        
        _serializeBundles();
        
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
    
    std::string BundleCatalog::_pathForBundleZipfile(const pBundle & b) const{
        return Functions::stringByAppendingPathComponent(_fetchedPath, b->suggestedStorageName()+".download");
    }
    std::string BundleCatalog::_pathForBundleStorageDirectory(const pBundle & b, bool tokenized) const{
        return Functions::stringByAppendingPathComponent(tokenized?kFetchedBundlesPathToken:_fetchedPath, b->suggestedStorageName())+"/";
    }

}}}