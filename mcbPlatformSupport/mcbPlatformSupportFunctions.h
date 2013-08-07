//
//  mcbPlatformSupportFunctions.h
//  mcb
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#ifndef __mcb__mcbPlatformSupportFunctions__
#define __mcb__mcbPlatformSupportFunctions__

#include "cocos2d.h"

namespace mcb{namespace PlatformSupport{namespace Functions{
    cocos2d::CCPoint relativeCoordinatesFromAbsolute(cocos2d::CCPoint absoluteCoordinates);
    cocos2d::CCPoint relativeScaleFromAbsolute(cocos2d::CCPoint absoluteScale);
    float normalizeAngle(float angle);
    template<class T>
    std::string t_to_string(T i){
        std::stringstream ss;
        std::string s;
        ss << i;
        s = ss.str();
        return s;
    }
    
    //parser
    cocos2d::CCPoint ccpFromString(const std::string & ccpString);
    std::string stringForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, const std::string & defaultVal="");
    bool boolForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, bool defaultVal=false);
    float floatForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, float defaultVal=0.f);
    cocos2d::CCPoint pointForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCPoint defaultVal={0.f,0.f});
    cocos2d::ccColor3B colorForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::ccColor3B defaultVal={0,0,0});
    cocos2d::ccColor3B webColorForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::ccColor3B defaultVal={0,0,0});
    cocos2d::CCTextAlignment textAlignmentForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCTextAlignment defaultVal=cocos2d::kCCTextAlignmentCenter);
    
    cocos2d::ccColor3B parseWebRGB(const std::string webColor);
    
}}}
#endif /* defined(__mcb__mcbPlatformSupportFunctions__) */
