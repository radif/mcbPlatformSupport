//
//  mcbSHA.h
//  crypto
//
//  Created by Radif Sharafullin on 1/20/14.
//  Copyright (c) 2014 Radif Sharafullin. All rights reserved.
//

#ifndef __mcb__crypto__mcbSHA__
#define __mcb__crypto__mcbSHA__

#include <iostream>
namespace mcb{namespace PlatformSupport{namespace crypto{
    std::string sha512(const std::string & str);
    std::string sha256(const std::string & str);
}}}
#endif /* defined(__mcb__crypto__mcbSHA__) */
