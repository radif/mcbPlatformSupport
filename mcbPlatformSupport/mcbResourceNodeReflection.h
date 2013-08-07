//
//  mcbResourceNodeReflection.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 6/9/13.
//
//

#ifndef __mcb__mcbResourceNodeReflection_h
#define __mcb__mcbResourceNodeReflection_h

namespace mcb{namespace PlatformSupport{
#include <string>
#include <map>
    
#define REGISTER_RESOURCE_NODE_CLASS_FOR_REFLECTION_H(classNameAsInPlist) \
static mcb::PlatformSupport::DerivedResourceNodeRegister<classNameAsInPlist> reg;\
static std::string className(){return #classNameAsInPlist;}\
virtual std::string stringFromClass(){return #classNameAsInPlist;}
    
    
#define REGISTER_RESOURCE_NODE_CLASS_FOR_REFLECTION_CPP(classNameAsInPlist) \
mcb::PlatformSupport::DerivedResourceNodeRegister<classNameAsInPlist> classNameAsInPlist::reg(#classNameAsInPlist)
    
    class ResourceNode;
    typedef std::map<std::string, ResourceNode*(*)()> layer_map_type;
    static layer_map_type * _mcbReflectionNodeMap(nullptr);
    
    class ResourceNodeFactory {
        friend class ResourceNode;
        static ResourceNode * createInstance(const std::string & s) {
            auto it(_reflectionMap()->find(s));
            if(it!=_reflectionMap()->end())
                return it->second();
            return nullptr;
        }
        
        template<typename T>
        static ResourceNode * createResourceNode() {
            T * retVal(new T());
            if (retVal){
                retVal->autorelease();
                return retVal;
            }
            delete retVal;
            retVal = nullptr;
            return nullptr;
        }
       
    protected:
        static layer_map_type * _reflectionMap() {

            static std::once_flag onceFlag;
            std::call_once(onceFlag,[](){
                if(!_mcbReflectionNodeMap)
                    _mcbReflectionNodeMap = new layer_map_type;
            });
            
            return _mcbReflectionNodeMap;
        }
        
    };
    
    template<typename T>
    class DerivedResourceNodeRegister : public ResourceNodeFactory {
    public:
        DerivedResourceNodeRegister(const std::string & s){
            _reflectionMap()->insert({s, &createResourceNode<T>});
#if defined (DEBUG) && DEBUG==1
            printf(" ---> reflection: registering node:-> %s\n",s.c_str());
#endif
        }
    };

}}

#endif/*(#ifndef __mcb__mcbResourceNodeReflection_h)*/
