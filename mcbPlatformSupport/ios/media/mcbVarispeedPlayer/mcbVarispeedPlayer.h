//
//  mcbVarispeedPlayer.h
//  mcb
//
//  Created by Radif Sharafullin on 9/12/15.
//  Copyright © 2015 Radif Sharafullin. All rights reserved.
//

#import <Foundation/Foundation.h>
@interface mcbVarispeedPlayer : NSObject

@property (nonatomic, readonly) BOOL loop;
@property (nonatomic, strong, nonnull, readonly)  NSString * audioPath;
@property (nonatomic, assign) float rate;
@property (nonatomic, readonly) BOOL isPlaying;
@property (nonatomic, copy, nullable) void (^onPlayToEnd)( mcbVarispeedPlayer * _Nonnull  player);

+(nonnull instancetype)playerWithAudioPath:(nonnull NSString*)audioPath loop:(BOOL)loop;

-(void)prepare;

-(void)setVolume:(const float)volume;
-(float)volume;

-(void)play;
-(void)pause;
-(void)stop;

-(float)currentTime;
-(float)duration;

@end
