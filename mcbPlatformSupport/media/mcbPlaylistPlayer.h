//
//  PlaylistPlayer.h
//  mcb
//
//  Created by Radif Sharafullin on 7/01/13.
//
//

#ifndef __mcb__PlaylistPlayer__
#define __mcb__PlaylistPlayer__

#include <string>
#include <vector>
#include <functional>
#include "mcbAudioPlayer.h"

namespace mcb{namespace PlatformSupport{
    
    class PlaylistPlayer{
        bool _isMeteringEnabled=false;
        const std::string _playlistName;
        pAudioPlayer _player=nullptr;
        const std::vector<std::string> _audioFiles;
        unsigned int _audioItemIndex=0;
        
        void _playNextItemInQueue();
        
    public:
        //lifecycle
        PlaylistPlayer(const std::string & playlistName, const std::vector<std::string> & audioFiles);
        virtual ~PlaylistPlayer();
        
        /* transport control */

        void play();            /* sound is played asynchronously. */
        void pause();           /* pauses playback, but remains ready to play. */
        void stop();            /* stops playback. no longer ready to play. */
        
        bool playNext();        /* force to play the next sound if available*/
        unsigned int currentSoundIndex() const;
        unsigned int totalSoundsCount() const;
        
        /* properties */
        bool getIsPlaying() const; /* is it playing or not? */
        
        void setMeteringEnabled(bool enabled);/* turns level metering on or off. default is off. */
        bool isMeteringEnabled() const;
        
        void updateMeters();/* call to refresh meter values */
        
        float getPeakPowerForChannel(int channelNumber); /* returns peak power in decibels for a given channel */
        float getAveragePowerForChannel(int channelNumber); /* returns average power in decibels for a given channel */
        
        
        //this is guaranted to be called
        std::function<void(PlaylistPlayer *player, const std::string & playlistName)> onPlaylistCompletedHandle=nullptr;
        
        //setup every player's property here, the m_playerCompletion and isMeteringEnabled will be overridden by this class. It is safe to form a weak_ptr to the player passed
        std::function<void(pAudioPlayer player, const std::string & audioFile, const unsigned int index)> onEachPlayerSetup=nullptr;
        
        void setSuspended(bool suspended);
        
    };
    
    typedef std::shared_ptr<PlaylistPlayer> pPlaylistPlayer;
    typedef std::weak_ptr<PlaylistPlayer> pWeakPlaylistPlayer;
}}

#endif /* defined(__mcb__PlaylistPlayer__) */
