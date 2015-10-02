//
//  mcbPlatformSupportFunctions.cpp
//  mcb
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#include "mcbPlatformSupportFunctions.h"
#include "mcbPlatformSupportConstants.h"
#include "mcbPlatformSupport.h"
#include "mcbNumber.h"
#include <iomanip>      // std::setprecision
#include <fstream> //file exists
#include <sys/stat.h> //mkdir
#include <sys/types.h>
#include <dirent.h>

using namespace cocos2d;
namespace mcb{namespace PlatformSupport{namespace Functions{
    
    static const std::string kmcbTRUEstring("true");
    static const std::string kmcbFALSEstring("false");
    
    bool boolFromString(const std::string & boolString){
        return boolString==kmcbTRUEstring || boolString=="TRUE" || boolString=="1" || boolString=="yes" || boolString=="YES";
    }
    const std::string & stringFromBool(bool value){
        return value?kmcbTRUEstring:kmcbFALSEstring;
    }
    
    cocos2d::CCPoint relativeCoordinatesFromAbsolute(cocos2d::CCPoint absoluteCoordinates){
        cocos2d::CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        cocos2d::CCRect visibleRect(mcb::PlatformSupport::getVisibleScreenRect());
        absoluteCoordinates.x=absoluteCoordinates.x/screenSize.width*visibleRect.size.width+visibleRect.origin.x;
        absoluteCoordinates.y=absoluteCoordinates.y/screenSize.height*visibleRect.size.height+visibleRect.origin.y;
        return absoluteCoordinates;
    }
    cocos2d::CCPoint relativeScaleFromAbsolute(cocos2d::CCPoint absoluteScale){
        cocos2d::CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        cocos2d::CCRect visibleRect(mcb::PlatformSupport::getVisibleScreenRect());
        absoluteScale.x=absoluteScale.x/screenSize.width*visibleRect.size.width;
        absoluteScale.y=absoluteScale.y/screenSize.height*visibleRect.size.height;
        return absoluteScale;
    }
    
    float normalizeAngle(float angle){
        if (angle > 0)
            angle = fmodf(angle, 360.0f);
        else
            angle = fmodf(angle, -360.0f);
        return angle;
    }
    float normalizeAngleRad(float angle){
        if (angle > 0)
            angle = fmodf(angle, 2.f*M_PI);
        else
            angle = fmodf(angle, -2.f*M_PI);
        return angle;
    }
    
    std::string formattedTimeFromSeconds(const float seconds, bool alwaysShowsHour, bool showsHundredth, bool hasSpaces){
        
        int time(seconds);
        
        long hour=time/3600;
        time=time%3600;
        int min=time/60;
        time=time%60;
        int sec=time;
        
        std::string space(hasSpaces? " ":"");
        
        std::stringstream ss;
        
        
        if (hour || alwaysShowsHour){
            std::string hourPrefix(hour<10?"0":"");
            ss<< hourPrefix<<hour << space<<":"<<space;
        }
        
        std::string minPrefix(min<10?"0":"");
        std::string secPrefix(sec<10?"0":"");
        ss<< minPrefix <<min << space <<":"<<space << secPrefix << sec;
        
        if (showsHundredth) {
            int hundredth((seconds-int(seconds))*100);
            std::string hPrefix(hundredth<10?"0":"");
            ss<<space<<":"<<space<<hPrefix<<hundredth;
        }
        
        
        return ss.str();
        
    }
    
    void replaceOccurrencesOfStringByString(std::string& str, const std::string& from, const std::string& to) {
        if(from.empty())
            return;
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }
    bool replaceFirstOccurrenceOfStringByString(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
    
    std::string lastPathComponent(const std::string & path) {
        size_t slashPos = path.find_last_of("/");
        if(slashPos != std::string::npos)
            return path.substr(slashPos + 1, path.length() - slashPos);
        return path;
    }
    std::string stringByDeletingLastPathComponent(const std::string & path){
        size_t slashPos = path.find_last_of("/");
        if(slashPos != std::string::npos)
            return path.substr(0, slashPos);
        return path;
    }
    std::string deletePathExtension(const std::string & path) {
        size_t dotPos = path.find_last_of(".");
        if(dotPos != std::string::npos)
            return path.substr(0, dotPos);
        return path;
    }
    std::string stringByAppendingPathComponent(std::string path, std::string component){
        _removeLastSlashInPath(path);
        _removeLastSlashInPath(component);
        return path+"/"+component;
    }
    bool stringHasEnding(std::string const &fullString, std::string const &ending){
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }
    void _removeLastSlashInPath(std::string & path){
        if (path.empty())
            return;
        size_t slashPos = path.find_last_of("/");
        while (slashPos==path.size()-1 && slashPos>0 && slashPos!=path.npos) {
            path=path.substr(0, path.size()-1);
            slashPos = path.find_last_of("/");
        }
    }
    
    //parser
    cocos2d::CCRect ccRectFromString(const std::string & ccRectString){
        cocos2d::CCRect retVal{0.f,0.f,0.f,0.f};
        do {
            std::string content(ccRectString);
            // find the first '{' and the third '}'
            size_t nPosRight(content.find('}'));
            CC_BREAK_IF(nPosRight==std::string::npos);
            
            // get the point string and size string
            std::string pointStr(content.substr(0, nPosRight+1));
            std::string sizeStr(content.substr(nPosRight + 1, content.length() - nPosRight));
            retVal.origin=ccpFromString(pointStr);
            auto sizeP(ccpFromString(sizeStr));
            retVal.size= {sizeP.x,sizeP.y};
        } while (false);
        
        return retVal;
    }
    cocos2d::CCPoint ccpFromString(const std::string & ccpString)
    {
        cocos2d::CCPoint p;
        
        cocos2d::CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        
        size_t loc = ccpString.find(Constants::kRelativePointOnScreen);
        size_t loc1 = ccpString.find(Constants::kRelativePositionFromAbsoluteString);
        size_t loc2 = ccpString.find(Constants::kRelativePositionFromCenter);
        
        if (loc != ccpString.npos) {
            std::string point = ccpString.substr(loc+1);
            p = cocos2d::CCPoint(CCPointFromString(point.c_str()));
            cocos2d::CCRect visibleRect(mcb::PlatformSupport::getVisibleScreenRect());
            p.x=p.x*visibleRect.size.width+visibleRect.origin.x;
            p.y=p.y*visibleRect.size.height+visibleRect.origin.y;
        }else if (loc1 != ccpString.npos) {
            std::string point(ccpString.substr(loc1+1));
            p = relativeCoordinatesFromAbsolute(cocos2d::CCPoint(CCPointFromString(point.c_str())));
        }else if (loc2 != ccpString.npos) {
            std::string point(ccpString.substr(loc1+1));
            p=cocos2d::ccpAdd(CCPointFromString(point.c_str()), mcb::PlatformSupport::getScreenCenter());
        }else if (ccpString==Constants::kCenterPositionString)
            p = cocos2d::CCPoint(screenSize.width/2, screenSize.height/2);
        else if (ccpString==Constants::kTopLeftPositionString)
            p = cocos2d::CCPoint(0, screenSize.height);
        else if (ccpString==Constants::kTopRightPositionString)
            p = cocos2d::CCPoint(screenSize.width, screenSize.height);
        else if (ccpString==Constants::kBottomLeftPositionString)
            p = cocos2d::CCPointZero;
        else if (ccpString==Constants::kBottomRightPositionString)
            p = cocos2d::CCPoint(screenSize.width, 0);
        else
            p = cocos2d::CCPoint(CCPointFromString(ccpString.c_str()));
        
        return p;
    }
    
    std::string stringForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, const std::string & defaultVal){
        CCString * s(dynamic_cast<CCString *>(dict->objectForKey(key.c_str())));
        if (!s)
            return defaultVal;
        return s->m_sString;
    }
    
    bool boolForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, bool defaultVal){
        bool retVal(defaultVal);
        CCObject * obj(dict->objectForKey(key.c_str()));
        PlatformSupport::Number * n(dynamic_cast<PlatformSupport::Number *>(obj));
        if (n) {
            retVal=n->value;
        }else{
            CCString * pString(dynamic_cast<CCString *>(obj));
            if (pString)
                retVal=pString->boolValue();
        }
        return retVal;
    }
    
    float floatForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, float defaultVal){
        float retVal(defaultVal);
        CCObject * obj(dict->objectForKey(key.c_str()));
        PlatformSupport::Number * n(dynamic_cast<PlatformSupport::Number *>(obj));
        if (n) {
            retVal=n->value;
        }else{
            CCString * pString(dynamic_cast<CCString *>(obj));
            if (pString)
                retVal=pString->floatValue();
        }
        return retVal;
    }
    
    cocos2d::CCSize sizeForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCSize defaultVal){
        CCPoint sizePoint(pointForObjectKey(dict, key, {defaultVal.width, defaultVal.height}));
        return {sizePoint.x, sizePoint.y};
    }
    cocos2d::CCPoint pointForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCPoint defaultVal){
        CCPoint retVal(defaultVal);
        CCObject * obj(dict->objectForKey(key.c_str()));
        CCString * pString(dynamic_cast<CCString *>(obj));
        if (pString)
            retVal=PlatformSupport::Functions::ccpFromString(pString->m_sString);
        return retVal;
    }
    cocos2d::CCRect rectForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCRect defaultVal){
        CCRect retVal(defaultVal);
        CCObject * obj(dict->objectForKey(key.c_str()));
        CCString * pString(dynamic_cast<CCString *>(obj));
        if (pString)
            retVal=PlatformSupport::Functions::ccRectFromString(pString->m_sString);
        return retVal;
    }
    cocos2d::ccColor3B colorForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::ccColor3B defaultVal){
        ccColor3B retVal(defaultVal);
        CCObject * obj(dict->objectForKey(key.c_str()));
        CCArray * colors(dynamic_cast<CCArray *>(obj));
        if (colors && colors->count()>2) {
            retVal.r=((Number *)colors->objectAtIndex(0))->value;
            retVal.g=((Number *)colors->objectAtIndex(1))->value;
            retVal.b=((Number *)colors->objectAtIndex(2))->value;
        }
        return retVal;
    }
    
    cocos2d::ccColor3B webColorForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::ccColor3B defaultVal){
        CCString * colorS(dynamic_cast<CCString *>(dict->objectForKey(key.c_str())));
        if (colorS)
            return parseWebRGB(colorS->m_sString);
        return defaultVal;
    }
    
    cocos2d::CCTextAlignment textAlignmentForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCTextAlignment defaultVal){
        CCTextAlignment retVal{defaultVal};
        CCObject * obj(dict->objectForKey(key.c_str()));
        CCString * str(dynamic_cast<CCString *>(obj));
        if (str) {
            if (str->m_sString=="left")
                retVal=kCCTextAlignmentLeft;
            else if (str->m_sString=="right")
                retVal=kCCTextAlignmentRight;
        }
        return retVal;
    }
    
    void setNodeProperties(cocos2d::CCNode * node, CCDictionary * data){
        
        CCPoint pos(Functions::pointForObjectKey(data, "position"));
        CCPoint posOffset(Functions::pointForObjectKey(data, "positionOffset"));
        pos=ccpAdd(pos, posOffset);
        
        float rotation(Functions::floatForObjectKey(data, "rotation"));
        
        //scale
        bool useScaleXY(false);
        float scaleX(1.f);
        float scaleY(1.f);
        
        float scale(1.f);
        Number * num((Number *)data->objectForKey("scale"));
        if (num)
            scale=*num;
        else{
            Number * pScaleX((Number *)data->objectForKey("scaleX"));
            Number * pScaleY((Number *)data->objectForKey("scaleY"));
            
            if (pScaleX || pScaleY) {
                useScaleXY=true;
                if (pScaleX)
                    scaleX=*pScaleX;
                if (pScaleY)
                    scaleY=*pScaleY;
            }
            
        }
        
        if (data->objectForKey("anchorPoint"))
            node->setAnchorPoint(Functions::pointForObjectKey(data, "anchorPoint", node->getAnchorPoint()));
        
        if (data->objectForKey("contentSize"))
            node->setContentSize(Functions::sizeForObjectKey(data, "contentSize", node->getContentSize()));
        
        node->setPosition(pos);
        
        if (useScaleXY){
            node->setScaleX(scaleX);
            node->setScaleY(scaleY);
        }else
            node->setScale(scale);
        
        node->setRotation(rotation);
        
        if (data->objectForKey("visible"))
            node->setVisible(Functions::boolForObjectKey(data, "visible", node->isVisible()));
        
    }
    
    
    //color
    unsigned int _parseHex(const char *pszValue){
        unsigned int uiValue = 0;
        
        if (pszValue && strlen(pszValue) >= 6){
            for (int i = 0; i < 6; i++)
                if (pszValue[i] >= '0' && pszValue[i] <= '9')
                    uiValue = (uiValue << 4) | (pszValue[i] - '0');
                else if (pszValue[i] >= 'a' && pszValue[i] <= 'f')
                    uiValue = (uiValue << 4) | (pszValue[i] - 'a' + 10);
                else if (pszValue[i] >= 'A' && pszValue[i] <= 'A')
                    uiValue = (uiValue << 4) | (pszValue[i] - 'A' + 10);
                else
                    uiValue = uiValue << 4;
        }
        
        return uiValue;
    }
    
    cocos2d::ccColor3B parseWebRGB(std::string webColor){
        std::transform(webColor.begin(), webColor.end(), webColor.begin(), ::tolower);
        const char *pszColor(webColor.c_str());
        unsigned int uiColor = 0;
        if (strlen(pszColor) > 1){
            if (pszColor[0] != '#')
                uiColor = atoi(pszColor + 1);
            else if (strlen(pszColor) >= 7)
                uiColor = _parseHex(pszColor + 1);
        }
        
        
        
        return cocos2d::ccColor3B{static_cast<GLubyte>(((uiColor >> 16) & 0xff)),
            static_cast<GLubyte>(((uiColor >> 8) & 0xff)),
            static_cast<GLubyte>(((uiColor)& 0xff))};
    }
    
    
    //random
    std::string generateRandomAlphanumericString(const int length, const bool appendTimestamp){
        const static std::string kAllChars("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
        const int allCharsSize(kAllChars.size());
        std::string retVal;
        for(int i(0);i<length;++i){
            int index(arc4random() % allCharsSize);
            retVal+=kAllChars[index];
        }
        if (appendTimestamp)
            retVal+="_"+t_to_string(time(0));
        return retVal;
    }
#pragma mark filesystem
    bool fileExists(const std::string& filePath){
        std::fstream file;
        file.open(filePath.c_str(), std::ios::in);
        if (file.is_open() == true){
            file.close();
            return true;
        }
        file.close();
        return false;
    }
    void copyFile(const std::string& filePathFrom, const std::string& filePathTo){
        assert(fileExists(filePathFrom));
        std::ifstream in (filePathFrom.c_str());
        std::ofstream out (filePathTo.c_str());
        out << in.rdbuf();
        out.close();
        in.close();
    }
    void renameFile(const std::string& filePathFrom, const std::string& filePathTo){
        std::rename(filePathFrom.c_str(), filePathTo.c_str());
    }
    void createDirectory(const std::string& dirPath){
        mkdir(dirPath.c_str(),0777);
    }
    void removeFile(const std::string& filePath){
        std::remove(filePath.c_str());
    }
    void removeDirectoryRecursive(std::string dirPath){
        _removeLastSlashInPath(dirPath);
        if (!isDirectory(dirPath))
            return;
        
        DIR*            dp;
        struct dirent*  ep;
        
        dp = opendir(dirPath.c_str());
        
        while ((ep = readdir(dp)) != NULL) {
            std::string name(ep->d_name);
            if (name!="." && name!="..") {
                std::string fullPath(stringByAppendingPathComponent(dirPath, name));
                if (isDirectory(fullPath))
                    removeDirectoryRecursive(fullPath);
                else
                    removeFile(fullPath);                
            }
        }
        closedir(dp);
        removeFile(dirPath.c_str());
    }
    bool isDirectory(const std::string & dirPath){
        struct stat s_buf;
        if (stat(dirPath.c_str(), &s_buf))
            return 0;
        return S_ISDIR(s_buf.st_mode);
    }
    void safeRemoveFileOrDirectoryAtPath(const std::string & path){
        if (fileExists(path)){
            if (isDirectory(path))
                removeDirectoryRecursive(path);
            else
                removeFile(path);
        }
    }
#pragma mark node geopmetry
    bool isVisibleOnScreen(cocos2d::CCNode *node){
        CCPoint lowerLeft, upperRight;
        return isVisibleOnScreen(node, lowerLeft, upperRight);
    }
    bool isVisibleOnScreen(cocos2d::CCNode *node, cocos2d::CCPoint & lowerLeft, cocos2d::CCPoint & upperRight){
        if (!node->getParent())
            return false;
        
        const CCPoint & pos(node->getPosition());
        const CCSize & sz(node->getContentSize());
        const CCPoint & ap(node->getAnchorPoint());
        
        lowerLeft={pos.x - sz.width * ap.x, pos.y - sz.height * ap.y};
        upperRight={lowerLeft.x + sz.width, lowerLeft.y + sz.height};
        lowerLeft=node->convertToWorldSpaceAR(lowerLeft);
        upperRight=node->convertToWorldSpaceAR(upperRight);
        
        CCRect visibleRect(getVisibleScreenRect());
        if(!(visibleRect.getMaxX() < lowerLeft.x ||
             upperRight.x < visibleRect.getMinX() ||
             visibleRect.getMaxY() < lowerLeft.y ||
             upperRight.y < visibleRect.getMinY()))
            return true;
        
        return false;
    }

#pragma mark actions
    cocos2d::CCSequence *actionSequenceFromVector(const std::vector<cocos2d::CCAction *> & actions, const int tag){
        cocos2d::CCSequence *retVal(nullptr);
        
        for (int i=0; i<actions.size(); i++) {
            cocos2d::CCSequence *anAction = (cocos2d::CCSequence *)actions.at(i);
            if (!retVal)
                retVal = anAction;
            else
                retVal = cocos2d::CCSequence::createWithTwoActions(retVal, anAction);
        }
        
        if (retVal)
            retVal->setTag(tag);
        
        return retVal;
        
    }

}}}
