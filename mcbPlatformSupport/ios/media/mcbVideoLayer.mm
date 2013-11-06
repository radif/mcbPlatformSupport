//
//  mcbVideoLayer.cpp
//  ColorDots
//
//  Created by Radif Sharafullin on 11/4/13.
//
//

#include "mcbVideoLayer.h"
#include "mcbPlatformSupport.h"
#import "EAGLView.h"

#import <AVFoundation/AVFoundation.h>

using namespace cocos2d;
@interface mcbVideoPlayer : UIView{
@public
    mcb::PlatformSupport::VideoLayer * videoLayer;
}
@end

@implementation mcbVideoPlayer{
    AVPlayer * _player;
}

-(instancetype)initWithFrame:(CGRect)frame{
    self = [super initWithFrame: frame];
    if (self) {
        self.userInteractionEnabled=FALSE;
        _player=[[AVPlayer alloc] init];
        self.backgroundColor = [UIColor clearColor];
        ((AVPlayerLayer *)self.layer).player=_player;
        //callbacks
        [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(onPlaybackFinished) name: AVPlayerItemDidPlayToEndTimeNotification object:nil];
    }
    return self;
}

-(void)dealloc{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [_player release];
    [super dealloc];
}
-(void)addToGLView{
    [[EAGLView sharedEGLView] addSubview:self];
}
-(void)removeFRomGLView{
    [self removeFromSuperview];
}
+(Class)layerClass {
	return [AVPlayerLayer class];
}
-(void)loadVideoFile:(NSString *)videoFile {
    [_player replaceCurrentItemWithPlayerItem:[AVPlayerItem playerItemWithURL:[NSURL fileURLWithPath:videoFile]]];
}
-(void)start{
    [_player play];
}
-(void)stop{
    [_player play];
}
-(void)pause{
    [_player pause];
}
-(void)resume{
    [_player play];
}
-(void)reset{
    [_player pause];
    [_player seekToTime: kCMTimeZero];
}
-(void)seek:(float)time{
    [_player seekToTime:CMTimeMakeWithSeconds(time, NSEC_PER_SEC)];
}
-(float)duration{
    return CMTimeGetSeconds([[_player currentItem] duration]);
}
-(float)currentTime{
    return CMTimeGetSeconds([[_player currentItem] currentTime]);
}
-(BOOL)isPlaying{
    return [_player rate]!=0.f;
}
//callbacks
- (void)onPlaybackFinished{
    if (videoLayer->onPlaybackFinished)
        videoLayer->onPlaybackFinished(videoLayer);
}

@end


namespace mcb{namespace PlatformSupport{
#define _nativePlayer static_cast<mcbVideoPlayer *>(_nativeHandler)
    
    CGRect CCRectToUI(const cocos2d::CCRect frame){
        CGRect retVal(CGRectMake(frame.origin.x, getVisibleScreenRect().size.height-frame.origin.y-frame.size.height, frame.size.width, frame.size.height));
        float scale(CCEGLView::sharedOpenGLView()->getScaleX()*.5f);
        retVal.origin.x*=scale;
        retVal.origin.y*=scale;
        retVal.size.width*=scale;
        retVal.size.height*=scale;
        return retVal;
    }
    
    VideoLayer::VideoLayer() : _videoFrame(getVisibleScreenRect()){

    }
    VideoLayer::~VideoLayer(){
        [_nativePlayer release];
    }
    bool VideoLayer::init(){
#if COCOS2D_VERSION <= 0x00020003
        auto retVal(true);
#else
        auto retVal(super::init());
#endif
        
        _nativeHandler=[[mcbVideoPlayer alloc] initWithFrame:[[EAGLView sharedEGLView] bounds]];
        _nativePlayer->videoLayer=this;
        
        return retVal;
    }
    void VideoLayer::onEnter(){
        super::onEnter();
        [_nativePlayer addToGLView];
    }
    void VideoLayer::onExit(){
        super::onExit();
        [_nativePlayer removeFRomGLView];
    }

    
#pragma mark control
    
    void VideoLayer::start(){
        [_nativePlayer start];
    }
    void VideoLayer::pause(){
        [_nativePlayer pause];
    }
    void VideoLayer::resume(){
        [_nativePlayer resume];
    }
    void VideoLayer::stop(){
        [_nativePlayer stop];
    }
    void VideoLayer::seek(float time){
        [_nativePlayer seek:time];
    }
    float VideoLayer::duration() const{
        return [_nativePlayer duration];
    }
    bool VideoLayer::isPlaying() const{
        return [_nativePlayer isPlaying];
    }
    float VideoLayer::currentTime() const{
        return [_nativePlayer currentTime];
    }
    
    const std::string & VideoLayer::videoPath() const{
        return _videoPath;
    }
    void VideoLayer::setVideoPath(const std::string & videoPath){
        _videoPath=videoPath;
        [_nativePlayer loadVideoFile:@(videoPath.c_str())];
    }
    
    void VideoLayer::setVideoFrame(const cocos2d::CCRect & videoFrame){
        _videoFrame=videoFrame;
        //CGRect fr(CCRectToUI(_videoFrame));
        //NSLog(@"%@",NSStringFromCGRect(fr));
        [_nativePlayer setFrame:CCRectToUI(_videoFrame)];
    }
    const cocos2d::CCRect & VideoLayer::videoFrame() const{
        return _videoFrame;
    }
    void VideoLayer::sizeToFill(const cocos2d::CCSize & originalSize){
        CGSize sz([_nativePlayer frame].size);
        
        float originalRatio(originalSize.width/originalSize.height);
        float screenRatio(sz.width/sz.height);
        
        float r(screenRatio/originalRatio);
        _nativePlayer.layer.transform=CATransform3DMakeScale(r, r, 1);
    }

}}