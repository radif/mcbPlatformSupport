//
//  mcbSoundPicker.h
//  mcb
//
//  Created by Radif Sharafullin on 8/4/13.
//
//

#ifndef __mcb__SoundPicker__
#define __mcb__SoundPicker__

#include <iostream>
#include <vector>

namespace cocos2d{
    class CCTexture2D;
}
namespace mcb{namespace PlatformSupport{namespace SoundPicker{
    
    class MediaItem{
        mutable cocos2d::CCTexture2D * _thumb=nullptr;
        const bool _isLocal;
        const std::string _localPath;
        
        mutable struct{
            std::string title;
            std::string sortingTitle;
            std::string artist;
            std::string sortingArtist;
            std::string albumTitle;
            std::string albumArtist;
            float duration=-1;
            std::string copiedFilePath;
        } _cache;
        
        
        cocos2d::CCTexture2D * _noArt=nullptr;
        
    public:
        void * _nativeHandle=nullptr;

        std::string persistentID() const;
        std::string assetURL() const;
        
        std::string title() const;
        const std::string & sortingTitle() const;
        const std::string & sortingArtist() const;

        std::string albumTitle() const;
        std::string artist() const;
        std::string albumArtist() const;
        
        float duration() const;
        cocos2d::CCTexture2D * artwork() const;
        cocos2d::CCTexture2D * thumb() const;
        
        void setNoArtwork(cocos2d::CCTexture2D * noartowrk);
        cocos2d::CCTexture2D * noArtwork() const;
        
        std::string copiedFilePath() const;//this one is returning a path only when the asset is copied by calling the method copyToLibrary()
        std::string localPath() const;
        bool isPlayable() const;
       
        float trackLength() const;
        std::string releaseDate() const;
        std::string releaseYearString() const;
        
        int trackNumber() const;
        
        std::string trackLengthString() const;
        bool assetNeedsToDownload() const;
        
        bool assetHasBeenDeleted() const;
        
        bool existsInLibrary() const;
        
        void copyToLibrary(const std::function<void(const std::string & copiedItemPath)> & completion) const;
        
        long hashL() const;
        
        MediaItem();
        MediaItem(const std::string & localFilePath);

        virtual ~MediaItem();
        
    };
    extern const char * kMediaLibraryUpdatedNotification;
    
    typedef std::shared_ptr<MediaItem> pMediaItem;
    typedef std::vector<pMediaItem> MediaItems;
    typedef std::shared_ptr<MediaItems> pMediaItems;
    
    void pickItemFromMusicLibrary(std::function<void(const std::string & itemFileCopyPath)> completion);
    std::string localPlaybackPath();
    void metadataForMediaFile(const std::string & filePath, const std::function<void(cocos2d::CCTexture2D * tex, const std::string & songTitle, const std::string & albumTitle, const std::string & artistName)> completion);

    
    //media items
    //use this handle or listen to the kMediaLibraryUpdatedNotification.
    //Only one handler is supported, for multiple listeners, use notifications
    void setMediaLibraryUpdatedHandle(const std::function<void()> & updateHaldle);
    
    pMediaItems queryMediaItems();
    
}}}

#endif /* defined(__mcb__SoundPicker__) */
