//
//  AudioPlayer.cpp
//  mcb
//
//  Created by Radif Sharafullin on 7/01/13.
//
//

#include "mcbAudioPlayer.h"
#include "mcbMainThreadCaller.h"

#import <AVFoundation/AVAudioPlayer.h>
#import "mcbVarispeedPlayer.h"

@interface IOSPlayerDelegate : NSObject<AVAudioPlayerDelegate>{
    @public
    std::function<void(bool)> m_playerFinishedHandle;
}
@end
@implementation IOSPlayerDelegate

-(id)init{
    self=[super init];
    if (self) {
        m_playerFinishedHandle=nullptr;
    }
    return self;
}
-(void)dealloc{
    m_playerFinishedHandle=nullptr;
    [super dealloc];
}
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag{
    if (m_playerFinishedHandle)
        m_playerFinishedHandle(flag);
}
/*
- (void)audioPlayerDecodeErrorDidOccur:(AVAudioPlayer *)player error:(NSError *)error{

}


- (void)audioPlayerBeginInterruption:(AVAudioPlayer *)player{

}

- (void)audioPlayerEndInterruption:(AVAudioPlayer *)player withOptions:(NSUInteger)flags{

}
 */
@end

namespace mcb{namespace PlatformSupport{
    
    struct PLayerHolder{
        AVAudioPlayer * player=nil;
        IOSPlayerDelegate * playerDelegate=nil;
        
        PLayerHolder(AVAudioPlayer * m_Player, IOSPlayerDelegate * m_PlayerDelegate){
            player=[m_Player retain];
            playerDelegate=[m_PlayerDelegate retain];
            player.delegate=playerDelegate;
        }
        
        virtual ~PLayerHolder(){
            if (player) {
                player.delegate=nil;
                if ([player isPlaying])
                    [player stop];
                [player release];
            }
            if (playerDelegate)
                [playerDelegate release];
        }
    };
    
#define playerHolder ((PLayerHolder *)__platformSpecificPlayerPointer)
#define iOSPlayer playerHolder->player
    
    void AudioPlayer::__platformSpecificAllocPlayer(){
        
        NSString * audioFilePath(@(_audioFile.c_str()));
        NSURL *url=[NSURL fileURLWithPath:audioFilePath];        
        AVAudioPlayer *player=[[AVAudioPlayer alloc] initWithContentsOfURL:url error:nil];
        IOSPlayerDelegate * playerDelegate=[IOSPlayerDelegate new];

        if (player) {
            /*
             @property BOOL enableRate NS_AVAILABLE(10_8, 5_0);  You must set enableRate to YES for the rate property to take effect. You must set this before calling prepareToPlay.
             
             */
            player.enableRate=TRUE;
            
        }
        
        
        playerDelegate->m_playerFinishedHandle=[=](bool successfully){
            __playerdidFinishPlaying(successfully);
            _callPlayerCompletion();
        };
        
        PLayerHolder * pHolder(new PLayerHolder(player, playerDelegate));
        
        __platformSpecificPlayerPointer=pHolder;
        
        
        [player release];
        [playerDelegate release];
    }
    void AudioPlayer::__platformSpecificDeletePlayer(){
        _callPlayerCompletion();
        PLayerHolder * pHolder(playerHolder);
        if (pHolder)
            delete pHolder;
        
        __platformSpecificPlayerPointer=nullptr;
    }
    
    
    bool AudioPlayer::prepareToPlay(){
        _playerCompletionIsCalled=false;
        return [iOSPlayer prepareToPlay];
    }
    void AudioPlayer::play(){
        if (_isSuspended)
            return;
        _playerCompletionIsCalled=false;
        if (!iOSPlayer)
            mcb::PlatformSupport::call_on_main_thread([=](){
                _callPlayerCompletion();
            });
        
        [iOSPlayer play];
    }
    void AudioPlayer::playAtTime(float time){
        if (_isSuspended)
            return;
        _playerCompletionIsCalled=false;
        [iOSPlayer playAtTime:time];
    }
    void AudioPlayer::pause(){
        if (_isSuspended)
            return;
        _callPlayerCompletion();
        [iOSPlayer pause];
    }
    void AudioPlayer::stop(){
        if (_isSuspended)
            return;
        _callPlayerCompletion();
        [iOSPlayer stop];
    }
    
    
    bool AudioPlayer::getIsPlaying() const{
        if (_isSuspended)
            return _wasPlayingBeforeSuspension;
        return [iOSPlayer isPlaying];
    }
    int AudioPlayer::getNumberOfChannels() const{
        return [iOSPlayer numberOfChannels];
    }
    float AudioPlayer::getDuration() const{
        return [iOSPlayer duration];
    }
    
    void AudioPlayer::setPan(float pan){
        if (_isSuspended)
            return;
        [iOSPlayer setPan:pan];
    }
    float AudioPlayer::getPan() const{
        return [iOSPlayer pan];
    }
    
    
    void AudioPlayer::setVolume(const float & volume){
        if (_isSuspended)
            return;
        [iOSPlayer setVolume:volume];
    }
    float AudioPlayer::getVolume() const{
        return [iOSPlayer volume];
    }
    
    void AudioPlayer::setRate(float rate){
        if (_isSuspended)
            return;
        [iOSPlayer setRate:rate];
    }
    float AudioPlayer::getRate() const{
        return [iOSPlayer rate];
    }
    
    void AudioPlayer::setCurrentTime(float currentTime){
        if (_isSuspended)
            return;
        [iOSPlayer setCurrentTime:currentTime];
    }
    float AudioPlayer::getCurrentTime() const{
        return [iOSPlayer currentTime];
    }
    
    
    float AudioPlayer::getDeviceCurrentTime() const{
        return [iOSPlayer deviceCurrentTime];
    }
    
    
    void AudioPlayer::setNumberOfLoops(int numberOfLoops){
        if (_isSuspended)
            return;
        [iOSPlayer setNumberOfLoops:numberOfLoops];
    }
    int AudioPlayer::getNumberOfLoops() const{
        return [iOSPlayer numberOfLoops];
    }
    
    
#pragma mark metering
    void AudioPlayer::setMeteringEnabled(bool enabled){
        [iOSPlayer setMeteringEnabled:enabled];
    }
    bool AudioPlayer::isMeteringEnabled(){
        return [iOSPlayer isMeteringEnabled];
    }
    
    void AudioPlayer::updateMeters(){
        [iOSPlayer updateMeters];
    }
    
    float AudioPlayer::getPeakPowerForChannel(int channelNumber){
        return [iOSPlayer peakPowerForChannel:channelNumber];
    }
    float AudioPlayer::getAveragePowerForChannel(int channelNumber){
        return [iOSPlayer averagePowerForChannel:channelNumber];
    }
    
#pragma mark completion
    void AudioPlayer::_callPlayerCompletion(){
        if (!_playerCompletionIsCalled) {
            _playerCompletionIsCalled=true;
            if (m_playerCompletion)
                m_playerCompletion(this);
        }
    }
    
    
#pragma mark -
#pragma mark VarispeedPlayer
#define _nativeVarispeedPlayer ((mcbVarispeedPlayer *)_nativeHandler)
    VarispeedPlayer::VarispeedPlayer(const std::string filePath, bool loop){
        mcbVarispeedPlayer * player=[mcbVarispeedPlayer playerWithAudioPath:@(filePath.c_str()) loop:loop];
        [player retain];
        _nativeHandler=player;
    }
    VarispeedPlayer::~VarispeedPlayer(){
        if (_nativeVarispeedPlayer) {
            [_nativeVarispeedPlayer release];
        }
    }
    
    void VarispeedPlayer::prepare(){
        [_nativeVarispeedPlayer prepare];
    }
    void VarispeedPlayer::play(){
        [_nativeVarispeedPlayer play];
    }
    void VarispeedPlayer::pause(){
        [_nativeVarispeedPlayer pause];
    }
    void VarispeedPlayer::stop(){
        [_nativeVarispeedPlayer stop];
    }
        
    void VarispeedPlayer::setRate(const float rate){
        [_nativeVarispeedPlayer setRate:rate];
    }
    float VarispeedPlayer::rate() const{
        return [_nativeVarispeedPlayer rate];
    }
        
    std::string VarispeedPlayer::audioPath() const{
        return [[_nativeVarispeedPlayer audioPath] UTF8String];
    }
    bool VarispeedPlayer::loop() const{
        return [_nativeVarispeedPlayer loop];
    }
   
}}

