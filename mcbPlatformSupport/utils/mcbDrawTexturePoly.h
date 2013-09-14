//
//  mcbDrawTexturePoly.h
//  mcb
//
//  Created by Radif Sharafullin on 7/30/13.
//
//

#ifndef __mcb__utils__DrawTexturePoly__
#define __mcb__utils__DrawTexturePoly__

#include "cocos2d.h"
namespace mcb{namespace PlatformSupport{namespace DrawTexturePoly{
    void drawTexturePoly(cocos2d::CCTexture2D * tex, const cocos2d::CCPoint * poli, const unsigned int numberOfPoints, const bool preferHorizontal, const float texScale=1.f);
}}}

#endif /* defined(__mcb__utils__DrawTexturePoly__) */
