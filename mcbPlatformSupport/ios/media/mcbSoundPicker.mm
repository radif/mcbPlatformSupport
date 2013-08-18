//
//  mcbSoundPicker.cpp
//  mcb
//
//  Created by Radif Sharafullin on 8/4/13.
//
//

#include "mcbSoundPicker.h"
#import <MediaPlayer/MediaPlayer.h>
#import <AVFoundation/AVFoundation.h>
#import "TSLibraryImport.h"
#include "CCTexture2D.h"
#include "CCImage.h"
#include <AudioToolbox/AudioToolbox.h>
#include "CCNotificationCenter.h"
namespace mcb{namespace PlatformSupport{ namespace SoundPicker{
    void copyMPMediaItemToLibrary(MPMediaItem * item, std::function<void(const std::string & copiedItemPath)> completion=nullptr);
}}}
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
    mcb::PlatformSupport::SoundPicker::copyMPMediaItemToLibrary(item, [=](const std::string & copiedItemPath){
        if (_completion)
            _completion(copiedItemPath);
    });
    
}

-(void)mediaPickerDidCancel:(MPMediaPickerController *)mediaPicker{
    [mediaPicker dismissViewControllerAnimated:TRUE completion:NULL];
}

@end



@interface MediaQueryHandler : NSObject
+(instancetype)sharedInstance;
@property(nonatomic, assign) std::function<void()> mediaLibraryUpdatedHandler;
-(mcb::PlatformSupport::SoundPicker::pMediaItems)queryMediaItems;
@end
@implementation MediaQueryHandler
static MediaQueryHandler * _sharedMediaHandler=nil;
+(instancetype)sharedInstance{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _sharedMediaHandler=[self new];
    });
    return _sharedMediaHandler;
}
-(instancetype)init{
    self=[super init];
    if (self) {
        _mediaLibraryUpdatedHandler=nullptr;
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(mediaLibraryUpdated:) name:MPMediaLibraryDidChangeNotification object:nil];
        [[MPMediaLibrary defaultMediaLibrary] beginGeneratingLibraryChangeNotifications];
    }
    return self;
}
-(void)dealloc{
    [[MPMediaLibrary defaultMediaLibrary] endGeneratingLibraryChangeNotifications];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}
- (void)mediaLibraryUpdated:(NSNotification *)notification {
    cocos2d::CCNotificationCenter::sharedNotificationCenter()->postNotification(mcb::PlatformSupport::SoundPicker::kMediaLibraryUpdatedNotification);
    if (_mediaLibraryUpdatedHandler)
        _mediaLibraryUpdatedHandler();
}
-(mcb::PlatformSupport::SoundPicker::pMediaItems)queryMediaItems{
    auto retVal(std::make_shared<mcb::PlatformSupport::SoundPicker::MediaItems>());
    
    MPMediaQuery * songsQuery([MPMediaQuery songsQuery]);
    NSArray * items([songsQuery items]);
    
    retVal->reserve(items.count);
    for (MPMediaItem * mediaItem in items) {
        mcb::PlatformSupport::SoundPicker::pMediaItem pItem(std::make_shared<mcb::PlatformSupport::SoundPicker::MediaItem>());
        pItem->_nativeHandle=[mediaItem retain];
        retVal->push_back(pItem);
    }
    
    /*
    
    MPMediaItemCollection *selectedMediaItemCollection = items[itemIndex];
    
    MPMediaItem *selectedMediaItem = [selectedMediaItemCollection representativeItem];
    
    NSMutableArray *songsArray = [NSMutableArray arrayWithCapacity:selectedMediaItemCollection.count];
    for (MPMediaItemCollection *mediaItemCollection in [self items]) {
        [songsArray addObject:[mediaItemCollection representativeItem]];
    }
    MPMediaItemCollection *mediaItemCollection = [MPMediaItemCollection collectionWithItems:songsArray];
    [self.delegate jgMediaQueryViewController:self didPickMediaItems:mediaItemCollection selectedItem:selectedMediaItem];
    
    */
    
    
    
    
    return retVal;
}
@end

namespace mcb{namespace PlatformSupport{ namespace SoundPicker{
    inline std::string stringFromNSString(NSString *s){return s? [s UTF8String]: "";}
    inline float floatFromNSNumber(NSNumber *n){return n? [n floatValue]: 0.f;}

#define nativeMediaItem static_cast<MPMediaItem *>(_nativeHandle)
#define nativeAsset static_cast<AVAsset *>(_nativeHandle)

    cocos2d::CCTexture2D * ccTextureFromNSData(NSData * data){
        cocos2d::CCTexture2D * retVal(nullptr);
        if (data) {
            unsigned char * buffer((unsigned char *)malloc(sizeof(unsigned char) * data.length));
            [data getBytes:buffer length:data.length];
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
            
            
            free(buffer);
        }
        return retVal;
    }
    
    
    std::string MediaItem::persistentID() const{
        if (_isLocal)
            return "";
        else{
            NSNumber * n([nativeMediaItem valueForProperty:MPMediaItemPropertyPersistentID]);
            if (n)
                return [[n stringValue] UTF8String];
            }
        return "";
    }
    std::string MediaItem::assetURL() const{
        if (_isLocal)
            return _localPath;
        else{
            NSURL * url([nativeMediaItem valueForProperty:MPMediaItemPropertyAssetURL]);
            if (url)
                return [[url absoluteString] UTF8String];
        }

        return "";
    }
    
    std::string MediaItem::title() const{
        if (_isLocal) {
            return _cache.title;
        }
        return stringFromNSString([nativeMediaItem valueForProperty:MPMediaItemPropertyTitle]);
    }
    std::string MediaItem::albumTitle() const{
        return stringFromNSString([nativeMediaItem valueForProperty:MPMediaItemPropertyAlbumTitle]);
    }
    std::string MediaItem::artist() const{
        if (_isLocal) {
            return _cache.artist;
        }

        return stringFromNSString([nativeMediaItem valueForProperty:MPMediaItemPropertyArtist]);
    }
    std::string MediaItem::albumArtist() const{
        return stringFromNSString([nativeMediaItem valueForProperty:MPMediaItemPropertyAlbumArtist]);
    }
    
    
    cocos2d::CCTexture2D * MediaItem::thumb() const{
        if (_thumb)
            return _thumb;

        if (_isLocal)
            return nullptr;
        
        
        UIImage *image(nil);
        MPMediaItemArtwork *artwork = [nativeMediaItem valueForProperty:MPMediaItemPropertyArtwork];
        if (artwork)
            image=[artwork imageWithSize:CGSizeMake(80.f, 80.f)];
        NSData * data(UIImagePNGRepresentation(image));
        cocos2d::CCTexture2D * retVal(ccTextureFromNSData(data));
        if (retVal) {
            _thumb=retVal;
            _thumb->retain();            
        }
        return retVal;
    }
    
    cocos2d::CCTexture2D * MediaItem::artwork() const{
        cocos2d::CCTexture2D * retVal(nullptr);
        if (_isLocal) {
            NSArray *metadata = [nativeAsset commonMetadata];
            for ( AVMetadataItem* item in metadata ) {
                @autoreleasepool {
                    NSString *key = [item commonKey];
                    if ([key isEqualToString:@"artwork"]){
                        NSData *data = [(NSDictionary *)[item value] objectForKey:@"data"];
                        if (data)
                            retVal=ccTextureFromNSData(data);
                    }
                }
            }
            
        }else{
            UIImage *image(nil);
            MPMediaItemArtwork *artwork = [nativeMediaItem valueForProperty:MPMediaItemPropertyArtwork];
            if (artwork)
                image=[artwork imageWithSize:artwork.bounds.size];
            
            NSData * data(UIImagePNGRepresentation(image));
            retVal=ccTextureFromNSData(data);
        }
        return retVal;
    }
    
    bool MediaItem::isPlayable() const{
        return !assetNeedsToDownload() && !assetHasBeenDeleted();
    }
    
    float MediaItem::trackLength() const{
        return [[nativeMediaItem valueForProperty:MPMediaItemPropertyPlaybackDuration] floatValue];
    }
    std::string MediaItem::releaseDate() const{
        NSDate * date([nativeMediaItem valueForProperty:MPMediaItemPropertyReleaseDate]);
        if (date) {
            NSDateFormatter * dateFormatter([[NSDateFormatter alloc] init]);
            [dateFormatter setDateFormat:@"yyyy-MM-dd 'at' HH:mm"];
            NSString * dateString([dateFormatter stringFromDate:date]);
            [dateFormatter release];
            return stringFromNSString(dateString);
        }
        return "";
    }
    std::string MediaItem::releaseYearString() const{
        return stringFromNSString([NSString stringWithFormat:@"%@", [nativeMediaItem valueForProperty:@"year"]]);
    }
    
    int MediaItem::trackNumber() const{
        return [[nativeMediaItem valueForProperty:MPMediaItemPropertyAlbumTrackNumber] intValue];
    }
    
    std::string MediaItem::trackLengthString() const{
        NSString *timeString = nil;
        const int secsPerMin = 60;
        const int minsPerHour = 60;
        const char *timeSep = ":";
        NSTimeInterval seconds = trackLength();
        seconds = floor(seconds);
        
        if(seconds < 60.0) {
            timeString = [NSString stringWithFormat:@"0:%02.0f", seconds];
        }
        else {
            int mins = seconds/secsPerMin;
            int secs = seconds - mins*secsPerMin;
            
            if(mins < 60.0) {
                timeString = [NSString stringWithFormat:@"%d%s%02d", mins, timeSep, secs];
            }
            else {
                int hours = mins/minsPerHour;
                mins -= hours * minsPerHour;
                timeString = [NSString stringWithFormat:@"%d%s%02d%s%02d", hours, timeSep, mins, timeSep, secs];
            }
        }
        return stringFromNSString(timeString);
    }
    bool MediaItem::assetNeedsToDownload() const{
        return assetURL().empty();
    }
    
    bool MediaItem::assetHasBeenDeleted() const{
        if (assetURL().empty())
            return false;
        else {
            NSString *urlString(@(assetURL().c_str()));
            bool assetURLPointsNowhere(([urlString rangeOfString:@"ipod-library://item/item.(null)"].location != NSNotFound));
            return assetURLPointsNowhere;
        }
    }
    
    bool MediaItem::existsInLibrary() const{
        MPMediaPropertyPredicate *predicate = [MPMediaPropertyPredicate predicateWithValue:@(persistentID().c_str())
                                                                               forProperty: MPMediaItemPropertyPersistentID];
        MPMediaQuery *query = [[MPMediaQuery alloc] init];
        [query addFilterPredicate:predicate];
        bool exists(([[query items] count] != 0));
        return exists;
    }
    
    void MediaItem::copyToLibrary(const std::function<void(const std::string & copiedItemPath)> & completion) const{
        if (_isLocal){
            if (completion)
                completion(_localPath);
        }else{
            copyMPMediaItemToLibrary(nativeMediaItem, [=](const std::string & copiedItemPath){
                if (completion)
                    completion(copiedItemPath);
            });
        }
    }
    
    MediaItem::MediaItem(): _isLocal(false), _localPath(""){}
    
    MediaItem::MediaItem(const std::string & localFilePath): _isLocal(true), _localPath(localFilePath){
        NSURL *fileURL = [NSURL fileURLWithPath:@(localFilePath.c_str())];
        AVAsset *asset = [AVURLAsset URLAssetWithURL:fileURL options:nil];
        [asset retain];
        _nativeHandle=asset;
        
        NSArray *metadata = [asset commonMetadata];
        for ( AVMetadataItem* item in metadata ) {
            @autoreleasepool {
                NSString *key = [item commonKey];
                NSString *value = [item stringValue];
                
                if ([key isEqualToString:@"title"])
                    _cache.title=stringFromNSString(value);
                else if ([key isEqualToString:@"albumName"])
                    _cache.albumTitle=stringFromNSString(value);
                else if ([key isEqualToString:@"artist"])
                    _cache.artist=stringFromNSString(value);
                else if ([key isEqualToString:@"artwork"]){
                    NSData *data = [(NSDictionary *)[item value] objectForKey:@"data"];
                    UIImage * image([UIImage imageWithData:data]);
                    //resize
                    CGSize newSize(CGSizeMake(80.f, 80.f));
                    UIGraphicsBeginImageContextWithOptions(newSize, NO, 0.0);
                    [image drawInRect:CGRectMake(0, 0, newSize.width, newSize.height)];
                    UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
                    UIGraphicsEndImageContext();
                    NSData * dt(UIImagePNGRepresentation(newImage));
                    if (dt){
                        _thumb=ccTextureFromNSData(dt);
                        _thumb->retain();
                    }
                }
            }
        }
        
    }
    
    MediaItem::~MediaItem(){
        CC_SAFE_RELEASE_NULL(_thumb);
        if (nativeMediaItem) {
            [nativeMediaItem release];
            _nativeHandle=nullptr;
        }
    }
    
    void pickItemFromMusicLibrary(std::function<void(const std::string & itemFileCopyPath)> completion){
        [mcbMedaiPickerHandler pickItemFromMusicLibrary:completion];
    }
    void metadataForMediaFile(const std::string & filePath, const std::function<void(cocos2d::CCTexture2D * tex, const std::string & songTitle, const std::string & albumTitle, const std::string & artistName)> completion){
        
        if (!completion)
            return;
        
        cocos2d::CCTexture2D * tex(nullptr);
        
        std::string songTitile, albumTitle, artistName;
        
        NSURL *fileURL = [NSURL fileURLWithPath:@(filePath.c_str())];
        AVAsset *asset = [AVURLAsset URLAssetWithURL:fileURL options:nil];
        
        
        NSArray *metadata = [asset commonMetadata];
        for ( AVMetadataItem* item in metadata ) {
            @autoreleasepool {
                NSString *key = [item commonKey];
                NSString *value = [item stringValue];
                
                if ([key isEqualToString:@"title"])
                    songTitile=stringFromNSString(value);
                else if ([key isEqualToString:@"albumName"])
                    albumTitle=stringFromNSString(value);
                else if ([key isEqualToString:@"artist"])
                    artistName=stringFromNSString(value);
                else if ([key isEqualToString:@"artwork"]){
                    NSData *data = [(NSDictionary *)[item value] objectForKey:@"data"];
                    if (data)
                        tex=ccTextureFromNSData(data);
                }
            }
        }
        
        if (completion)
            completion(tex,songTitile,albumTitle, artistName);
        
    }

    
    
    
    void setMediaLibraryUpdatedHandle(const std::function<void()> & updateHaldle){
        [[MediaQueryHandler sharedInstance] setMediaLibraryUpdatedHandler:updateHaldle];
    }
    
    pMediaItems queryMediaItems(){
        return [[MediaQueryHandler sharedInstance] queryMediaItems];
    }
    
    void copyMPMediaItemToLibrary(MPMediaItem * item, std::function<void(const std::string & copiedItemPath)> completion){
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
                    if (completion)
                        completion(localPath);
                });
            }];
        }else{
            //drm?
        }
    }

}}}