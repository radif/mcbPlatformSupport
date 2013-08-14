//
//  mcbSoundPicker.cpp
//  mcb
//
//  Created by Radif Sharafullin on 8/4/13.
//
//

#include "mcbSoundPicker.h"
#import <MediaPlayer/MPMediaPickerController.h>
#import <AVFoundation/AVFoundation.h>
#import "TSLibraryImport.h"
#include "CCTexture2D.h"
#include "CCImage.h"
#include <AudioToolbox/AudioToolbox.h>

@interface mcbMedaiPickerHandler : NSObject <MPMediaPickerControllerDelegate>
+(void)pickItemFromMusicLibrary:(std::function<void(const std::string & itemFileCopyPath)>) completion;
@end

@implementation mcbMedaiPickerHandler{
    std::function<void(const std::string & itemFileCopyPath)> _completion;
}
+(void)pickItemFromMusicLibrary:(std::function<void(const std::string & itemFileCopyPath)>) completion{
    __attribute__((unused)) mcbMedaiPickerHandler * instance([[self alloc] initWithItemFromMusicLibrary:completion]);
}

-(instancetype)initWithItemFromMusicLibrary:(std::function<void(const std::string & itemFileCopyPath)>) completion{
    self=[super init];
    if (self) {
        [self retain];
        _completion=completion;
        MPMediaPickerController *mediaPicker = [[MPMediaPickerController alloc] initWithMediaTypes:MPMediaTypeMusic];
        mediaPicker.delegate = self;
        mediaPicker.allowsPickingMultipleItems = FALSE; // this is the default
        [[[self class] rootViewController] presentViewController:mediaPicker animated:TRUE completion:NULL];
        [mediaPicker release];
    }
    return self;
}
-(void)dealloc{
    NSLog(@"%s", __FUNCTION__);
    [super dealloc];
}

+(UIViewController *)rootViewController{
    UIViewController *rootViewController = nil;
    id appDelegate = [[UIApplication sharedApplication] delegate];
    if ([appDelegate respondsToSelector:@selector(viewController)])
    {
        rootViewController = [appDelegate valueForKey:@"viewController"];
    }
    if (!rootViewController && [appDelegate respondsToSelector:@selector(window)])
    {
        UIWindow *window = [appDelegate valueForKey:@"window"];
        rootViewController = window.rootViewController;
    }
    return rootViewController;
}
#pragma mark picker delegate

-(void)mediaPicker:(MPMediaPickerController *) mediaPicker didPickMediaItems:(MPMediaItemCollection *)collection {
    
    [mediaPicker dismissViewControllerAnimated:TRUE completion:NULL];
    
    
    MPMediaItem *item = [[collection items] objectAtIndex:0];
    
    
    NSURL *assetURL = [item valueForProperty:MPMediaItemPropertyAssetURL];
    
    if (assetURL) {
        // create destination URL
        std::string localPath(mcb::PlatformSupport::SoundPicker::localPlaybackPath());
        
        NSURL* outURL = [NSURL fileURLWithPath:@(localPath.c_str())];
        // we're responsible for making sure the destination url doesn't already exist
        [[NSFileManager defaultManager] removeItemAtURL:outURL error:nil];
        
        // create the import object
        TSLibraryImport* import = [[TSLibraryImport alloc] init];
        [import importAsset:assetURL toURL:outURL completionBlock:^(TSLibraryImport* import) {
            /*
             * If the export was successful (check the status and error properties of
             * the TSLibraryImport instance) you know have a local copy of the file
             * at `outURL` You can get PCM samples for processing by opening it with
             * ExtAudioFile. Yay!
             *
             * Here we're just playing it with AVPlayer
             */
            if (import.status != AVAssetExportSessionStatusCompleted) {
                // something went wrong with the import
                NSLog(@"Error importing: %@", import.error);
                [import release];
                import = nil;
                return;
            }
            
            // import completed
            [import release];
            import = nil;
            dispatch_async(dispatch_get_main_queue(), ^{
                if (_completion)
                    _completion(localPath);
            });
        }];
    }else{
        //drm?
    }
}

-(void)mediaPickerDidCancel:(MPMediaPickerController *)mediaPicker{
    [mediaPicker dismissViewControllerAnimated:TRUE completion:NULL];
}
@end

namespace mcb{namespace PlatformSupport{ namespace SoundPicker{
    void pickItemFromMusicLibrary(std::function<void(const std::string & itemFileCopyPath)> completion){
        [mcbMedaiPickerHandler pickItemFromMusicLibrary:completion];
    }
    void metadataForMediaFile(const std::string & filePath, const std::function<void(cocos2d::CCTexture2D * tex, const::std::string & songTitle, const::std::string & albumTitle, const::std::string & artistName)> completion){
        
        if (!completion)
            return;
        
        NSURL *fileURL = [NSURL fileURLWithPath:@(filePath.c_str())];
        AVAsset *asset = [AVURLAsset URLAssetWithURL:fileURL options:nil];
        
        NSArray *keys = [NSArray arrayWithObjects:@"commonMetadata", nil];
        [asset loadValuesAsynchronouslyForKeys:keys completionHandler:^{
            NSArray *artworks = [AVMetadataItem metadataItemsFromArray:asset.commonMetadata withKey:AVMetadataCommonKeyArtwork keySpace:AVMetadataKeySpaceCommon];
            
            
            auto textureFromData([](NSData * data)->cocos2d::CCTexture2D *{
                cocos2d::CCTexture2D * retVal(nullptr);
                if (data) {
                    void * buffer;
                    [data getBytes:buffer];
                    cocos2d::CCImage * image(new cocos2d::CCImage);
                    if(image && image->initWithImageData(buffer, data.length))
                        image->autorelease();
                    else{
                        CC_SAFE_DELETE(image);
                        image=nullptr;
                    }
                    
                    if (image) {
                        retVal=new cocos2d::CCTexture2D;
                        
                        if (retVal && retVal->initWithImage(image))
                            retVal->autorelease();
                        else{
                            CC_SAFE_DELETE(retVal);
                            retVal=nullptr;
                        }
                        
                        
                    }
                    
                    
                    
                }
                
                return retVal;
            });
            
            cocos2d::CCTexture2D * tex(nullptr);
            std::string songTitile, albumTitle, artistName;
            
            
            ExtAudioFileRef extAFRef;
            
            OSStatus err;
            CFURLRef inpUrl = (CFURLRef)fileURL;
            err = ExtAudioFileOpenURL(inpUrl, &extAFRef);
            if(err == noErr) {
                AudioFileID afid;
                AudioFileOpenURL(inpUrl, kAudioFileReadPermission, 0, &afid);
                UInt32 size = 0;
                UInt32 writable;
                OSStatus error = AudioFileGetPropertyInfo(afid, kAudioFilePropertyInfoDictionary, &size, &writable);
                if ( error == noErr ) {
                    CFDictionaryRef info = NULL;
                    error = AudioFileGetProperty(afid, kAudioFilePropertyInfoDictionary, &size, &info);
                    if ( error == noErr ) {
                        NSLog(@"file properties: %@", (NSDictionary *)info);
                        NSDictionary *dict = (NSDictionary *)info;
                        NSString *idTitle = [dict valueForKey:@"title"];
                        NSString *idArtist = [dict valueForKey:@"artist"];
                        if (idTitle)
                            songTitile=[idTitle UTF8String];

                        if (idArtist)
                            artistName=[idArtist UTF8String];

                    }
                    if(info) CFRelease(info);
                } else {
                    NSLog(@"Error reading tags");
                }
                AudioFileClose(afid);
            }
            
           
            
            
            
            //TODO: artowrks?
            for (AVMetadataItem *item in artworks) {
                @autoreleasepool {
                    if ([item.keySpace isEqualToString:AVMetadataKeySpaceID3]) {
                        NSDictionary *dict = [item.value copyWithZone:nil];//leaking? do we need autorelease
                        [dict autorelease];
                        NSData * dt([dict objectForKey:@"data"]);
                        if (dt)
                            tex=textureFromData(dt);
                        
                    }else if ([item.keySpace isEqualToString:AVMetadataKeySpaceiTunes]) {
                        NSData * dt([item.value copyWithZone:nil]);
                        [dt autorelease];
                        if (dt)
                            tex=textureFromData(dt);
                    }else if ([item.keySpace isEqualToString:AVMetadataiTunesMetadataKeySongName]){
                        NSString * song([item.value copyWithZone:nil]);
                        [song autorelease];
                        songTitile=[song UTF8String];
                    }else if ([item.keySpace isEqualToString:AVMetadataiTunesMetadataKeyAlbum]){
                        NSString * album([item.value copyWithZone:nil]);
                        [album autorelease];
                        albumTitle=[album UTF8String];
                    }else if ([item.keySpace isEqualToString:AVMetadataiTunesMetadataKeyArtist]){
                        NSString * artist([item.value copyWithZone:nil]);
                        [artist autorelease];
                        artistName=[artist UTF8String];
                    }
                }
            }
            
            if (completion)
                completion(tex,songTitile,albumTitle, artistName);
            
        }];
        
        
        /*
         extAFReachedEOF = NO;
         OSStatus err;
         CFURLRef inpUrl = (CFURLRef)audioURL;
         err = ExtAudioFileOpenURL(inpUrl, &extAFRef);
         if(err != noErr) {
         [self status:ERROR message:@"Cannot open audio file"];
         return;
         }
         
         AudioFileID afid;
         AudioFileOpenURL(inpUrl, kAudioFileReadPermission, 0, &afid);
         UInt32 size = 0;
         UInt32 writable;
         OSStatus error = AudioFileGetPropertyInfo(afid, kAudioFilePropertyInfoDictionary, &size, &writable);
         if ( error == noErr ) {
         CFDictionaryRef info = NULL;
         error = AudioFileGetProperty(afid, kAudioFilePropertyInfoDictionary, &size, &info);
         if ( error == noErr ) {
         NSLog(@"file properties: %@", (NSDictionary *)info);
         NSDictionary *dict = (NSDictionary *)info;
         NSString *idTitle = [dict valueForKey:@"title"];
         NSString *idArtist = [dict valueForKey:@"artist"];
         if(idTitle != nil && idArtist != nil) {
         [_title release];
         _title = [[NSString stringWithFormat:@"%@ - %@",idArtist, idTitle]retain];
         } else if(idTitle != nil) {
         [_title release];
         _title = [idTitle copy];
         }
         }
         if(info) CFRelease(info);
         } else {
         NSLog(@"Error reading tags");
         }
         AudioFileClose(afid);
         */
    }
}}}