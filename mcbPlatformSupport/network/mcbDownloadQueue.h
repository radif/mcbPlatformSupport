//
//  mcbDownloadQueue.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/4/14.
//
//

#ifndef __SoundSurfer__mcbDownloadQueue__
#define __SoundSurfer__mcbDownloadQueue__

#include <mcbFactory.h>
#include "mcbDownloadTask.h"

namespace mcb{namespace PlatformSupport{namespace network{
    class DownloadQueue : public SingletonFactory<DownloadQueue>{
        int _maxConcurrentDownloads=8;
        std::vector<pDownloadTask> _queue;
    public:
        void setMaxConcurrentDownloads(int maxConcurrentDownloads);
        int maxConcurrentDownloads() const{return _maxConcurrentDownloads;}
        
        bool isDownloading(const HTTPRequest & request) const;
        
        bool enqueueDownload(const HTTPRequest & request, const std::string & saveToPath, const NetworkTask::completion_handle_t & completion, const NetworkTask::progress_handle_t & progress=nullptr);
        
        void cancelDownload(const HTTPRequest & request);
        void cancelAllDownloads();
        
        void circulate();
        
        void setTimeoutForConnect(int value);
        int timeoutForConnect();
        void setTimeoutForRead(int value);
        int timeoutForRead();
        virtual void init() override;

    };
}}}
#endif /* defined(__SoundSurfer__mcbDownloadQueue__) */
