//
//  mcbOpenALPlayer.cpp
//  EndlessReader
//
//  Created by Radif Sharafullin on 8/12/13.
//
//

#include "mcbOpenALPlayer.h"
#import <ObjectAL.h>

namespace mcb{namespace PlatformSupport{

#define getNativeHolder static_cast<OpenALPlayerNativeiVarHolder *>(__nativeHolder)
    struct OpenALPlayerNativeiVarHolder {
        ALSource* source=nil;
        ALBuffer* buffer=nil;
        virtual ~OpenALPlayerNativeiVarHolder(){
            [buffer release];
            [source release];
        }
    };
    
    void OpenALPlayer::__platformSpecificInit(const std::string & audioFile, bool loops){
        __nativeHolder=new OpenALPlayerNativeiVarHolder;
        
        
        // We'll let OALSimpleAudio deal with the device and context.
        // Since we're not going to use it for playing effects, don't give it any sources.
        [OALSimpleAudio sharedInstance].reservedSources = 10;
        // Turn on reverb.
        [OALSimpleAudio sharedInstance].context.listener.reverbOn=_useReverb;
        
        
        if (_useReverb) {
            // Set global reverb level to "normal" (0dB)
            [OALSimpleAudio sharedInstance].context.listener.globalReverbLevel=0;
            [OALSimpleAudio sharedInstance].context.listener.reverbRoomType = ALC_ASA_REVERB_ROOM_TYPE_SmallRoom;
        }

        getNativeHolder->source = [[ALSource source] retain];
        

        // "Pan" uses OpenAL positioning, so we have to force ColdFunk.caf from stereo to mono.
        getNativeHolder->buffer = [[[OpenALManager sharedInstance] bufferFromFile:@(audioFile.c_str()) reduceToMono:_useReverb] retain];
       // getNativeHolder->buffer= [[[OALSimpleAudio sharedInstance] preloadEffect:@(audioFile.c_str()) reduceToMono:YES]retain];
        
        
        
    }
    void OpenALPlayer::__platformSpecificCleanup(){
        delete getNativeHolder;
    }

    
    void OpenALPlayer::play(){
        [getNativeHolder->source play:getNativeHolder->buffer loop:_loops];
    }

    void OpenALPlayer::stop(){
        [getNativeHolder->source stop];
    }
    
    void OpenALPlayer::setReverbObstruction(const float reverbObstruction){
        getNativeHolder->source.reverbObstruction=reverbObstruction;
    }
    void OpenALPlayer::setReverbOcclusion(const float reverbOcclusion){
        getNativeHolder->source.reverbOcclusion=reverbOcclusion;
    }
    void OpenALPlayer::setReverbSendLevel(const float reverbSendLevel){
        getNativeHolder->source.reverbSendLevel=reverbSendLevel;

    }
    bool OpenALPlayer::getIsPlaying() const{
        return getNativeHolder->source.playing;
    }

    float OpenALPlayer::getDuration() const{
        return getNativeHolder->buffer.duration;
    }
    
    void OpenALPlayer::setGlobalReverbLevel(const float reverbLevel){
        [OALSimpleAudio sharedInstance].context.listener.globalReverbLevel=reverbLevel;
    }
    void OpenALPlayer::setRoomType(const int roomType){
        [OALSimpleAudio sharedInstance].context.listener.reverbRoomType=roomType;
    }
    float OpenALPlayer::getReverbObstruction() const{
        return getNativeHolder->source.reverbObstruction;
    }
    float OpenALPlayer::getReverbOcclusion() const{
        return getNativeHolder->source.reverbOcclusion;
    }
    float OpenALPlayer::getReverbSendLevel() const{
        return getNativeHolder->source.reverbSendLevel;
    }
    
    void OpenALPlayer::setPan(float pan){
        getNativeHolder->source.pan=pan;
    }
    float OpenALPlayer::getPan() const{
        return getNativeHolder->source.pan;
    }
    
    void OpenALPlayer::setVolume(const float & volume){
    	getNativeHolder->source.gain=volume;
    }
    float OpenALPlayer::getVolume() const{
        return getNativeHolder->source.gain;
    }
    
    void OpenALPlayer::setPitch(float pitch){
    	getNativeHolder->source.pitch=pitch;
    }
    float OpenALPlayer::getPitch() const{
        return getNativeHolder->source.pitch;
    }
    
    
    void OpenALPlayer::setCurrentTime(float currentTime){
        getNativeHolder->source.offsetInSeconds=currentTime;
    }
    
    void OpenALPlayer::setDirection(const Direction & d){
        getNativeHolder->source.direction=ALVector{d.x,d.y,d.z};
    }
    OpenALPlayer::Direction OpenALPlayer::getDirection() const{
        auto dir(getNativeHolder->source.direction);
        return {dir.x, dir.y, dir.z};
    }

    
    float OpenALPlayer::getCurrentTime() const{
        return getNativeHolder->source.offsetInSeconds;
        [getNativeHolder->source stop];
    }
    
#pragma mark static
    void OpenALPlayer::playEffect(const std::string & audioFile){
        [[OALSimpleAudio sharedInstance] playEffect:@(audioFile.c_str())];
    }
    void OpenALPlayer::preloadEffect(const std::string & audioFile){
        [[OALSimpleAudio sharedInstance] preloadEffect:@(audioFile.c_str())];
    }

}}