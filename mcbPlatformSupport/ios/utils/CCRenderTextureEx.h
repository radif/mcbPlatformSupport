//
//  CCRenderTextureEx.h
//  BubblePOP
//
//  Created by Jae hong Ahn on 13. 9. 9..
//
//

#ifndef __BubblePOP__CCRenderTextureEx__
#define __BubblePOP__CCRenderTextureEx__

#include "cocos2d.h"
#import <Foundation/Foundation.h>

NS_CC_BEGIN

class CCRenderTextureEx : public CCRenderTexture{
public:
    CGImage* newCGImage();
    UIImage* getUIImage();
    
    /** initializes a RenderTexture object with width and height in Points and a pixel format( only RGB and RGBA formats are valid ) and depthStencil format*/
    static CCRenderTextureEx * create(int w ,int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat);
    
    /** creates a RenderTexture object with width and height in Points and a pixel format, only RGB and RGBA formats are valid */
    static CCRenderTextureEx * create(int w, int h, CCTexture2DPixelFormat eFormat);
    
    /** creates a RenderTexture object with width and height in Points, pixel format is RGBA8888 */
    static CCRenderTextureEx * create(int w, int h);
};


NS_CC_END

#endif /* defined(__BubblePOP__CCRenderTextureEx__) */
