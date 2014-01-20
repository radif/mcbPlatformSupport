//
//  mcbSHA.h
//  sha
//
//  Created by Radif Sharafullin on 1/20/14.
//  Copyright (c) 2014 Radif Sharafullin. All rights reserved.
//

#ifndef __sha__mcbSHA__
#define __sha__mcbSHA__

#include <iostream>
namespace mcb{namespace PlatformSupport{namespace crypto{
    std::string sha512(const std::string & str);
    std::string sha256(const std::string & str);
}}}
#endif /* defined(__sha__mcbSHA__) */
