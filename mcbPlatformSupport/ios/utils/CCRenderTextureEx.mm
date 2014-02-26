//
//  CCRenderTextureEx.cpp
//  BubblePOP
//
//  Created by Jae hong Ahn on 13. 9. 9..
//
//

#include "CCRenderTextureEx.h"

NS_CC_BEGIN

CGImage* CCRenderTextureEx::newCGImage()
{
    CCAssert(m_ePixelFormat == kCCTexture2DPixelFormat_RGBA8888,"only RGBA8888 can be saved as image");
	
    CCSize ccSize = m_pTexture->getContentSizeInPixels();
    
	CGSize s = CGSizeMake(ccSize.width, ccSize.height);//[_texture contentSizeInPixels];
	int tx = s.width;
	int ty = s.height;
	
	int bitsPerComponent			= 8;
    int bitsPerPixel                = 4 * 8;
    int bytesPerPixel               = bitsPerPixel / 8;
	int bytesPerRow					= bytesPerPixel * tx;
	NSInteger myDataLength			= bytesPerRow * ty;
	
	GLubyte *buffer	= (GLubyte *)calloc(myDataLength,1);
	GLubyte *pixels	= (GLubyte *)calloc(myDataLength,1);
	
	
	if( ! (buffer && pixels) ) {
		CCLOG("cocos2d-x: CCRenderTexture#getCGImageFromBuffer: not enough memory");
		free(buffer);
		free(pixels);
		return nil;
	}
	
	this->begin();
	
	glReadPixels(0,0,tx,ty,GL_RGBA,GL_UNSIGNED_BYTE, buffer);
    
	this->end();
	
	// make data provider with data.
	
	CGBitmapInfo bitmapInfo	= kCGImageAlphaPremultipliedLast | kCGBitmapByteOrderDefault;
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer, myDataLength, NULL);
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	CGImageRef iref	= CGImageCreate(tx, ty,
									bitsPerComponent, bitsPerPixel, bytesPerRow,
									colorSpaceRef, bitmapInfo, provider,
									NULL, false,
									kCGRenderingIntentDefault);
	
	CGContextRef context = CGBitmapContextCreate(pixels, tx,
												 ty, CGImageGetBitsPerComponent(iref),
												 CGImageGetBytesPerRow(iref), CGImageGetColorSpace(iref),
												 bitmapInfo);
	
	// vertically flipped
	if( YES ) {
		CGContextTranslateCTM(context, 0.0f, ty);
		CGContextScaleCTM(context, 1.0f, -1.0f);
	}
	CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, tx, ty), iref);
	CGImageRef image = CGBitmapContextCreateImage(context);
	
	CGContextRelease(context);
	CGImageRelease(iref);
	CGColorSpaceRelease(colorSpaceRef);
	CGDataProviderRelease(provider);
	
	free(pixels);
	free(buffer);
	
	return image;
}

UIImage* CCRenderTextureEx::getUIImage()
{
    CGImageRef imageRef = this->newCGImage();
	
	UIImage* image	= [[UIImage alloc] initWithCGImage:imageRef scale:CC_CONTENT_SCALE_FACTOR() orientation:UIImageOrientationUp];
    
	CGImageRelease( imageRef );
    
	return [image autorelease];
}




//creatior
CCRenderTextureEx * CCRenderTextureEx::create(int w, int h, CCTexture2DPixelFormat eFormat)
{
    CCRenderTextureEx *pRet = new CCRenderTextureEx();
    
    if(pRet && pRet->initWithWidthAndHeight(w, h, eFormat))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

CCRenderTextureEx * CCRenderTextureEx::create(int w ,int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat)
{
    CCRenderTextureEx *pRet = new CCRenderTextureEx();
    
    if(pRet && pRet->initWithWidthAndHeight(w, h, eFormat, uDepthStencilFormat))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

CCRenderTextureEx * CCRenderTextureEx::create(int w, int h)
{
    CCRenderTextureEx *pRet = new CCRenderTextureEx();
    
    if(pRet && pRet->initWithWidthAndHeight(w, h, kCCTexture2DPixelFormat_RGBA8888, 0))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}


NS_CC_END