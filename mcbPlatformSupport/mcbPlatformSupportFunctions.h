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
#include <iomanip>
#include <locale>

namespace mcb{namespace PlatformSupport{namespace Functions{
    cocos2d::CCPoint relativeCoordinatesFromAbsolute(cocos2d::CCPoint absoluteCoordinates);
    cocos2d::CCPoint relativeScaleFromAbsolute(cocos2d::CCPoint absoluteScale);
    float normalizeAngle(float angle);
    float normalizeAngleRad(float angle);
    
    std::string formattedTimeFromSeconds(const float seconds, bool alwaysShowsHour=false, bool showsHundredth=false, bool hasSpaces=true);
    
    //converting string to types
    bool boolFromString(const std::string & boolString);
    static int intFromString(const std::string & intString){return std::atoi(intString.c_str());}
    static long longFromString(const std::string & longString){return std::atol(longString.c_str());}
    static long long longlongFromString(const std::string & longlongString){return std::atoll(longlongString.c_str());}
    static float floatFromString(const std::string & floatString){return std::atof(floatString.c_str());}
    
    //converting typed to string
    const std::string & stringFromBool(bool value);
    template<class T>
    std::string t_to_string(T i){
        std::stringstream ss;
        std::string s;
        ss << i;
        s = ss.str();
        return s;
    }
    
    //numeric formats
    template<class T>
    std::string numberFormatWithCommas(T value){
        struct Numpunct: public std::numpunct<char>{
        protected:
            virtual char do_thousands_sep() const{return ',';}
            virtual std::string do_grouping() const{return "\03";}
        };
        std::stringstream ss;
        ss.imbue({std::locale(), new Numpunct});
        ss << std::setprecision(2) << std::fixed << value;
        return ss.str();
    }
    
    //string manipulations
    void replaceOccurrencesOfStringByString(std::string& str, const std::string& from, const std::string& to);
    bool replaceFirstOccurrenceOfStringByString(std::string& str, const std::string& from, const std::string& to) ;
    
    std::string lastPathComponent(const std::string & path);
    std::string stringByDeletingLastPathComponent(const std::string & path);
    std::string deletePathExtension(const std::string & path);
    std::string stringByAppendingPathComponent(std::string path, std::string component);
    bool stringHasEnding(std::string const &fullString, std::string const &ending);
    bool stringIsNumber(const std::string & s);
    //TODO: Canonical path
    void _removeLastSlashInPath(std::string & path);
    
    
    //filesystem
    bool fileExists(const std::string& filePath);
    void copyFile(const std::string& filePathFrom, const std::string& filePathTo);
    void renameFile(const std::string& filePathFrom, const std::string& filePathTo);
    void createDirectory(const std::string& dirPath);
    void removeFile(const std::string& filePath);
    void removeDirectoryRecursive(std::string dirPath);
    bool isDirectory(const std::string & dirPath);
    void safeRemoveFileOrDirectoryAtPath(const std::string & path);
    
    //parser
    
    //TODO: use std::optional or boost::optional for return types and default vals
    
    cocos2d::CCPoint ccpFromString(const std::string & ccpString);
    cocos2d::CCRect ccRectFromString(const std::string & ccRectString);
    std::string stringForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, const std::string & defaultVal="");
    bool boolForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, bool defaultVal=false);
    float floatForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, float defaultVal=0.f);
    cocos2d::CCDictionary * dictionaryForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCDictionary * defaultVal=nullptr);

    cocos2d::CCSize sizeForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCSize defaultVal={0.f,0.f});
    cocos2d::CCPoint pointForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCPoint defaultVal={0.f,0.f});
    cocos2d::CCRect rectForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCRect defaultVal={0.f,0.f,0.f,0.f});
    cocos2d::ccColor3B colorForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::ccColor3B defaultVal={0,0,0});
    cocos2d::ccColor3B webColorForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::ccColor3B defaultVal={0,0,0});
    cocos2d::CCTextAlignment textAlignmentForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCTextAlignment defaultVal=cocos2d::kCCTextAlignmentCenter);
    void setNodeProperties(cocos2d::CCNode * node, cocos2d::CCDictionary * data);
    
    cocos2d::ccColor3B parseWebRGB(std::string webColor);
    static cocos2d::ccColor4F color4FFrom4B(const cocos2d::ccColor4B &color){return {color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f};}
    static cocos2d::ccColor4B color4BFrom4F(const cocos2d::ccColor4F &color){return {static_cast<GLubyte>(color.r*255.f), static_cast<GLubyte>(color.g*255.f), static_cast<GLubyte>(color.b*255.f), static_cast<GLubyte>(color.a*255.f)};}
    static cocos2d::ccColor3B color3BFrom4B(const cocos2d::ccColor4B &color){return {color.r, color.g, color.b};}
    static cocos2d::ccColor4B color4BFrom3B(const cocos2d::ccColor3B &color){return {color.r, color.g, color.b, 255};}
    
    static cocos2d::ccColor4F color4FFrom3B(const cocos2d::ccColor3B &color){return {color.r/255.f, color.g/255.f, color.b/255.f, 1.f};}
    static cocos2d::ccColor3B color3BFrom4F(const cocos2d::ccColor4F &color){return {static_cast<GLubyte>(color.r*255.f), static_cast<GLubyte>(color.g*255.f), static_cast<GLubyte>(color.b*255.f)};}
    
    
    static cocos2d::ccColor4F colorAdd(const cocos2d::ccColor4F &color1, const cocos2d::ccColor4F &color2){return {color1.r+color2.r, color1.g+color2.g, color1.b+color2.b, color1.a+color2.a};}
    
    static cocos2d::ccColor4F colorSub(const cocos2d::ccColor4F &color1, const cocos2d::ccColor4F &color2){return {color1.r-color2.r, color1.g-color2.g, color1.b-color2.b, color1.a-color2.a};}
    
    static cocos2d::ccColor4F colorMult(const cocos2d::ccColor4F &color1, const cocos2d::ccColor4F &color2){return {color1.r*color2.r, color1.g*color2.g, color1.b*color2.b, color1.a*color2.a};}
    static cocos2d::ccColor4F colorMult(const cocos2d::ccColor4F &color1, const float ratio){return {color1.r*ratio, color1.g*ratio, color1.b*ratio, color1.a*ratio};}
    
    
    std::string generateRandomAlphanumericString(const int length=16, const bool appendTimestamp=true);
    
    //node geometry
    bool isVisibleOnScreen(cocos2d::CCNode *node);//I have CCSprite in mind in 99% of the cases
    bool isVisibleOnScreen(cocos2d::CCNode *node, cocos2d::CCPoint & bottomLeft, cocos2d::CCPoint & topRight);//I have CCSprite in mind in 99% of the cases

    //actions
    typedef std::vector<cocos2d::CCAction *> actions_t;
    cocos2d::CCSequence *actionSequenceFromVector(const actions_t & actions, const int tag=0);
    
}}}
#endif /* defined(__mcb__mcbPlatformSupportFunctions__) */
