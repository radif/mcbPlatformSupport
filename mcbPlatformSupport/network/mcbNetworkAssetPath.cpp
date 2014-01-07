//
//  mcbNetworkAssetPath.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/5/14.
//
//

#include "mcbNetworkAssetPath.h"
namespace mcb{namespace PlatformSupport{namespace network{
    void NetworkAssetManager::init(){
    
    }
    std::string NetworkAssetManager::resolveNetworkAssetPath(const std::string & path){
        std::string localPath;
        std::string networkURL;
        
        //check the data, see if can find it, if not, return local path, otherwise, return path from data, if exists
        
        //enqeue:
        //compare versions, start download, unzip
        
        //remove previous version, update data
        
    }
    void NetworkAssetManager::loadVersionsWithPath(const std::string & path){
        std::string dataPath(resolveNetworkAssetPath(path));
        
    }
}}}