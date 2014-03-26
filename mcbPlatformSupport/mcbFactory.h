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
#include "mcbPlatformSupportFunctions.h"

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
        
        static C * createWithBundlePath(const std::string & path){
            std::string p(PlatformSupport::resolvePath(path));
            Functions::_removeLastSlashInPath(p);
            cocos2d::CCDictionary * data(dictionaryFromPlist(Functions::stringByAppendingPathComponent(p, PlatformSupport::Constants::kManifestDataFile)));
            return createInstance(p, data);
        }
    protected:
        virtual bool init(cocos2d::CCDictionary * data)=0;
        virtual ~Factory()=default;
    };
    
    
    template<class C>
    class SingletonFactory{
    public:
        static C * sharedInstance(){
            static C * sharedInstance(nullptr);
            if(!sharedInstance){
                //CLANG BUG - needed to remove thread safety with std::once
                //                static std::once_flag onceFlag;
                //                std::call_once(onceFlag,[](){
                sharedInstance=new C;
                sharedInstance->init();
                //                });
            }
            return sharedInstance;
        }
    protected:
        virtual void init()=0;
        virtual ~SingletonFactory()=default;
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
