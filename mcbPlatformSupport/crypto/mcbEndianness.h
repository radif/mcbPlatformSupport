//
//  mcbEndianness.h
//  crypto
//
//  Created by Radif Sharafullin on 1/20/14.
//  Copyright (c) 2014 Radif Sharafullin. All rights reserved.
//

#ifndef __mcb__crypto__mcbEndianness__
#define __mcb__crypto__mcbEndianness__

#include <iostream>

#if CHAR_BIT != 8
#error "unsupported char size"
#endif
namespace mcb{namespace PlatformSupport{namespace crypto{
    bool isLittleEndian();
    bool isBigEndian();
    bool isPDPEndian();
}}}
#endif /* defined(__mcb__crypto__mcbEndianness__) */
