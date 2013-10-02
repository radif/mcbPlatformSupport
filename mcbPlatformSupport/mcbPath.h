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
        std::unique_ptr<path_tokens_t> _tokens=nullptr;
    protected:
        Path(const std::string & localPath): _localPath(localPath){}
        Path(){};
        std::string _localPath;
        void addTokenForPath(const std::string token, const std::string path){if (!_tokens) _tokens=std::unique_ptr<path_tokens_t>(new path_tokens_t); (*_tokens)[token]=path;}
        std::string mcbPath(const std::string & inPath){return PlatformSupport::resolvePath(inPath, _localPath, _tokens.get());}
        cocos2d::CCString * mcbPath(const cocos2d::CCString * inPath){return cocos2d::CCString::create(mcbPath(inPath->m_sString));}
    };
}}

#endif
