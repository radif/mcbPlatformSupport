//
//  mcbPath.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 6/9/13.
//
//

#ifndef mcb_mcbPath_h
#define mcb_mcbPath_h

#include "mcbPlatformSupport.h"

namespace mcb{namespace PlatformSupport{
    class Path{
    protected:
        Path(const std::string & localPath): _localPath(localPath){}
        Path(){};
        std::string _localPath;
        
        std::string mcbPath(const std::string & inPath){return PlatformSupport::resolvePath(inPath, _localPath);}
        cocos2d::CCString * mcbPath(const cocos2d::CCString * inPath){return cocos2d::CCString::create(mcbPath(inPath->m_sString));}
    };
}}

#endif
