//
//  mcbNetworkAssetPath.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/5/14.
//
//

#ifndef __SoundSurfer__mcbNetworkAssetPath__
#define __SoundSurfer__mcbNetworkAssetPath__

#include "mcbFactory.h"
#include "mcbPath.h"
#include "mcbHTTPRequest.h"

namespace mcb{namespace PlatformSupport{namespace network{
    
    void downloadAndUnzipBundle(const HTTPRequest & request, const std::string & bundlesDirectory, const std::function<void(const std::string & bundlePath, bool success)> & completion, const std::function<void(float progress)> & progress=nullptr);
    
    class NetworkAssetManager : public SingletonFactory<NetworkAssetManager>{
    public:
        virtual void init() override;
        
        //track file versions
        //local versions, compare and resolve path
        
        //initWithVersions
        
        void loadVersionsWithPath(const std::string & path);
        
        //path format:
        //remote | fallback, remote path is the key for local path storage. If fallback not provided "" will be returned immediately if the assets has not downloaded yet
        //#(http://path.com?file.zip|$(SHARED)/store)/asset.jpg
        std::string resolveNetworkAssetPath(const std::string & path);
        
    };
    
    class NetworkAssetPath : public virtual Path{
    protected:
        std::string mcbNetworkAssetPath(const std::string & inPath){return mcbPath(NetworkAssetManager::sharedInstance()->resolveNetworkAssetPath(inPath));}
        cocos2d::CCString * mcbNetworkAssetPath(const cocos2d::CCString * inPath){return cocos2d::CCString::create(mcbNetworkAssetPath(inPath->m_sString));}
    };
    
   
   
}}}
#endif /* defined(__SoundSurfer__mcbNetworkAssetPath__) */
