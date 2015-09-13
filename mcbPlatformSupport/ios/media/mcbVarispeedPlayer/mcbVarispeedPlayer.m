//
//  mcbVarispeedPlayer.m
//  mcb
//
//  Created by Radif Sharafullin on 9/12/15.
//  Copyright © 2015 Radif Sharafullin. All rights reserved.
//

#import "mcbVarispeedPlayer.h"
#import <AVFoundation/AVFoundation.h>

#if !__has_feature(objc_arc)
#error "Enable ARC! by adding -fobjc-arc flag"
#endif

@implementation mcbVarispeedPlayer{
    __strong AVPlayer * _player;
}
-(instancetype)init{
    if (self=[super init]) {
        _rate=1.f;
    }
    return self;
}
+(nonnull instancetype)playerWithAudioPath:(nonnull NSString*)audioPath loop:(BOOL)loop{
    mcbVarispeedPlayer * retVal=[mcbVarispeedPlayer new];
    retVal->_audioPath=audioPath;
    retVal->_loop=loop;
    return retVal;
}
-(void)prepare{
    if (!_player) {
        NSURL * url=[NSURL fileURLWithPath:_audioPath];
        AVAsset * asset=[AVURLAsset URLAssetWithURL:url options:nil];
        AVPlayerItem *playerItem = [AVPlayerItem playerItemWithAsset:asset];
        playerItem.audioTimePitchAlgorithm = AVAudioTimePitchAlgorithmVarispeed;
        _player = [AVPlayer playerWithPlayerItem:playerItem];
        [_player addObserver:self forKeyPath:@"status" options:0 context:nil];

        //loop
        __weak typeof(_player) weakPlayer = _player;
        __weak typeof(self) weakSelf = self;
        NSNotificationCenter *noteCenter = [NSNotificationCenter defaultCenter];
        [noteCenter addObserverForName:AVPlayerItemDidPlayToEndTimeNotification
                                object:nil
                                 queue:nil
                            usingBlock:^(NSNotification *note) {
                                if (note.object==weakPlayer.currentItem)
                                
                                    if (weakSelf.onPlayToEnd)
                                        weakSelf.onPlayToEnd(weakSelf);
                                
                                    if (weakSelf.loop) {
                                        [weakPlayer seekToTime:kCMTimeZero];
                                        [weakPlayer play];
                                        weakPlayer.rate=weakSelf.rate;
                                    }
                            }];
    }
}
-(void)play{
    if (!_player)
        [self prepare];
    [_player play];
    _player.rate=_rate;
}
-(void)pause{
    bool oldLoop=_loop;
    _loop=false;
    [_player pause];
    _loop=oldLoop;
}
-(void)stop{
    bool oldLoop=_loop;
    _loop=false;
    [_player pause];
    [_player seekToTime:kCMTimeZero];
    _loop=oldLoop;
}
-(BOOL)isPlaying{
    return [_player rate] != 0.f;
}
-(void)setRate:(float)rate{
    if (self.isPlaying)
        _player.rate=rate;
    _rate=rate;
}
-(void)dealloc{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [_player removeObserver:self forKeyPath:@"status"];
    [_player pause];
    _player=nil;
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
    if (object == _player && [keyPath isEqualToString:@"status"]) {
        if (_player.status == AVPlayerStatusFailed) {
            NSLog(@"VarispeedPlayer Failed");
        } else if (_player.status == AVPlayerStatusReadyToPlay) {
            NSLog(@"VarispeedPlayer Ready to Play");
        } else if (_player.status == AVPlayerItemStatusUnknown) {
            NSLog(@"VarispeedPlayer Unknown");
        }
    }
}
@end
