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
}}}