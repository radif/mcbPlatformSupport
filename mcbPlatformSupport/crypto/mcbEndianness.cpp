//
//  mcbEndianness.cpp
//  crypto
//
//  Created by Radif Sharafullin on 1/20/14.
//  Copyright (c) 2014 Radif Sharafullin. All rights reserved.
//

#include "mcbEndianness.h"
#include <limits.h>
#include <stdint.h>

namespace mcb{namespace PlatformSupport{namespace crypto{
    enum{O32_LITTLE_ENDIAN = 0x03020100ul, O32_BIG_ENDIAN = 0x00010203ul, O32_PDP_ENDIAN = 0x01000302ul};
    static const union { unsigned char bytes[4]; uint32_t value; } o32_host_order{{0, 1, 2, 3 }};
    
    bool isLittleEndian(){
        return o32_host_order.value==O32_LITTLE_ENDIAN;
    }
    bool isBigEndian(){
        return o32_host_order.value==O32_BIG_ENDIAN;
    }
    bool isPDPEndian(){
        return o32_host_order.value==O32_PDP_ENDIAN;
    }
}}}