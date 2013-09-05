//
//  mcbFactory.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 6/11/13.
//
//

#ifndef __mcb__mcbFactory__
#define __mcb__mcbFactory__

#include "mcbPath.h"
#include "mcbPlatformSupport.h"

namespace mcb{namespace PlatformSupport{
    
    //Simply subclass your node from it passing itself as a template parameter.
    //eg: class AudioWaveNode : public cocos2d::CCNode, public PlatformSupport::Factory<AudioWaveNode>{
    //this will give you a create function returning your type
    //In case you need to derive from your class further, use using Factory<yourType>
    //class Derived  : public AudioWaveNode, public PlatformSupport::Factory<Derived>{
    //public:
    //using PlatformSupport::Factory<Derived>::createInstance;
    //...
    
    
    template<class C>
    class Factory : protected virtual Path{
    public:
        static C * createInstance(const std::string & localPath, cocos2d::CCDictionary * data){
            auto retVal(new C);
            if (retVal) {
                retVal->_localPath=PlatformSupport::resolvePath(localPath);
                if (retVal->init(data))
                    retVal->autorelease();
                else{
                    if(retVal)
                        retVal->release();
                    retVal=nullptr;
                }
            }
            return retVal;
        }
    protected:
        virtual bool init(cocos2d::CCDictionary * data)=0;
        virtual ~Factory(){}
    };
    
    
    //universal create function outside of a class
    template<typename C, typename ...Args>
    static C * create(Args&& ...args) {
        auto retVal(new C);
        if (retVal && retVal->init(std::forward<Args>(args)...)) {
            retVal->autorelease();
        }else{
            delete retVal;
            retVal=nullptr;
        }
        return retVal;
    }
}}

#endif /* defined(__mcb__mcbFactory__) */
