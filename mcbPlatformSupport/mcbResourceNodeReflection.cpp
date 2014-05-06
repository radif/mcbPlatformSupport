//
//  mcbResourceNodeReflection.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 5/6/14.
//
//

#include "mcbResourceNodeReflection.h"
#include <future>
namespace mcb{namespace PlatformSupport{
    static layer_map_type * _mcbReflectionNodeMap(nullptr);
    layer_map_type * ResourceNodeFactory::_reflectionMap() {
        static std::once_flag onceFlag;
        std::call_once(onceFlag,[](){
            if(!_mcbReflectionNodeMap)
                _mcbReflectionNodeMap=new layer_map_type;
        });
        return _mcbReflectionNodeMap;
    }
    
    ResourceNode * ResourceNodeFactory::createInstance(const std::string & s) {
        auto it(_reflectionMap()->find(s));
        if(it!=_reflectionMap()->end())
            return it->second();
        return nullptr;
    }

}}

