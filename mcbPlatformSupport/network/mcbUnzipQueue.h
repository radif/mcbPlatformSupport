//
//  mcbUnzipQueue.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/4/14.
//
//

#ifndef __SoundSurfer__mcbUnzipQueue__
#define __SoundSurfer__mcbUnzipQueue__

#include "mcbFactory.h"
#include <mcbUnzipTask.h>

namespace mcb{namespace PlatformSupport{namespace network{
    class UnzipQueue : public SingletonFactory<UnzipQueue>{
        int _maxConcurrentOperations=1;
        std::vector<pUnzipTask> _queue;
    public:
        void setMaxConcurrentOperations(int maxConcurrentOperations);
        int maxConcurrentOperations() const{return _maxConcurrentOperations;}
        
        void enqueueUnzip(const std::string & archivedFile, const std::string & destinationPath, const UnzipTask::completion_handle_t & completion, const UnzipTask::progress_handle_t & progress=nullptr, const UnzipTask::completed_each_t & completedEach=nullptr);
        
        
        void circulate();
        
        virtual void init() override;
    };
}}}

#endif /* defined(__SoundSurfer__mcbUnzipQueue__) */
