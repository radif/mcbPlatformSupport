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
    
    std::string formattedTimeFromSeconds(const float seconds, bool alwaysShowsHour=false, bool showsHundredth=false, bool hasSpaces=true);
    
    template<class T>
    std::string t_to_string(T i){
        std::stringstream ss;
        std::string s;
        ss << i;
        s = ss.str();
        return s;
    }
    
    template<class T>
    std::string numberFormatWithCommas(T value){
        struct : public std::numpunct<char>{
        protected:
            virtual char do_thousands_sep() const{return ',';}
            virtual std::string do_grouping() const{return "\03";}
        } numpunct;
        std::stringstream ss;
        ss.imbue({std::locale(), &numpunct});
        ss << std::setprecision(2) << std::fixed << value;
        return ss.str();
    }
    
    //parser
    
    template <typename T>
    struct containerOrNull{
        bool isSet=false;
        T value;
        //assignement
        containerOrNull(T && newVal){set(std::forward<T>(newVal));}
        containerOrNull(T & newVal){set(newVal);}
        containerOrNull(T newVal){set(std::move(newVal));}

        virtual ~containerOrNull(){}
        
        void set(T && newVal){value=std::forward<T>(newVal); isSet=true;}
        void set(T & newVal){value=newVal; isSet=true;}
        void setV(T newVal){value=std::move(newVal); isSet=true;}
        
        //comparison
        bool operator == (const containerOrNull<T> & other) const{return value==other.value && isSet==other.isSet;}

        //assignment
        containerOrNull<T>& operator = (const containerOrNull<T> & other){value=other.value; isSet=other.isSet; return *this;}
        
        T & operator = (const T & other){value=other; isSet=true; return value;}
        T & operator = (const T && other){value=std::forward<T>(other); isSet=true; return value;}
        
        //TODO: move
        
        //TODO: copy
        
        //get
        T get() const{return value;}
        T* getPtr() const{if (isSet) return &value; return nullptr;}
        T* operator *() const{return getPtr();}
    };
    
    
    cocos2d::CCPoint ccpFromString(const std::string & ccpString);
    std::string stringForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, const std::string & defaultVal="");
    bool boolForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, bool defaultVal=false);
    float floatForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, float defaultVal=0.f);
    cocos2d::CCSize sizeForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCSize defaultVal={0.f,0.f});
    cocos2d::CCPoint pointForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCPoint defaultVal={0.f,0.f});
    cocos2d::ccColor3B colorForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::ccColor3B defaultVal={0,0,0});
    cocos2d::ccColor3B webColorForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::ccColor3B defaultVal={0,0,0});
    cocos2d::CCTextAlignment textAlignmentForObjectKey(cocos2d::CCDictionary *dict, const std::string & key, cocos2d::CCTextAlignment defaultVal=cocos2d::kCCTextAlignmentCenter);
    void setNodeProperties(cocos2d::CCNode * node, cocos2d::CCDictionary * data);
    
    cocos2d::ccColor3B parseWebRGB(std::string webColor);
    
    
    
}}}
#endif /* defined(__mcb__mcbPlatformSupportFunctions__) */
