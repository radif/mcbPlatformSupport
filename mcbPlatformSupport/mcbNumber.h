//
//  mcbNumber.h
//  mcb
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#ifndef __mcb__Number__
#define __mcb__Number__

#include "cocos2d.h"
namespace mcb{namespace PlatformSupport{
    class Number : public cocos2d::CCObject
    {
    public:
        double value;
        static Number * create(double v=0){
            Number *n=new Number(v);
            n->autorelease();
            return n;
        }
        Number(double v=0):cocos2d::CCObject(), value(v){}
        void setValue(double v) {value = v;}
        operator bool(){return (bool)value;}
        operator int() {return (int)value;}
        operator unsigned() {return (unsigned)value;}
        operator float() {return (float)value;}
        Number& operator = (double val) {
            value = val;
            return *this;
        }
        Number& operator ++() {
            value = (int)value + 1;
            return *this;
        }
        };
}}
    
#endif /* defined(__mcb__Number__) */
