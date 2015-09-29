//
//  mcbOpenALPlayer.cpp
//  EndlessReader
//
//  Created by Radif Sharafullin on 8/12/13.
//
//

#include "mcbOpenALPlayer.h"
namespace mcb{namespace PlatformSupport{
    OpenALPlayer::OpenALPlayer(const std::string & audioFile, bool useReverb, bool loops)
    :_useReverb(useReverb)
    , _audioPath(audioFile)
    , _loops(loops)
    {
        __platformSpecificInit(audioFile, loops);
    }
    OpenALPlayer::~OpenALPlayer(){
        __platformSpecificCleanup();
    }
}}