//
//  mcbUnzipQueue.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/4/14.
//
//

#include "mcbUnzipQueue.h"
namespace mcb{namespace PlatformSupport{namespace network{
    void UnzipQueue::init(){
        
    }
    void UnzipQueue::setMaxConcurrentOperations(int maxConcurrentOperations){
        _maxConcurrentOperations=maxConcurrentOperations;
    }
    
    void UnzipQueue::enqueueUnzip(const std::string & archivedFile, const std::string & destinationPath, const UnzipTask::completion_handle_t & completion, const UnzipTask::progress_handle_t & progress, const UnzipTask::completed_each_t & completedEach){
        pUnzipTask task(UnzipTask::create(archivedFile, destinationPath, [=](UnzipTask::Status status){
            if (completion)
                completion(status);
            //remove from queue
            circulate();

        }, progress, completedEach));
        _queue.emplace_back(task);
        circulate();
        
    }
    
    
    void UnzipQueue::circulate(){
        //remove unused
        decltype(_queue) filtered;
        for (const auto & task : _queue)
            if (!task->isFinished())
                filtered.emplace_back(task);
        
        //std::swap(_queue, filtered);
        _queue=std::move(filtered);
        
        //launch up to max concurent
        int count(fminf(_queue.size(), _maxConcurrentOperations));
        for (int i(0); i<count; ++i)
            if (!_queue.at(i)->hasStarted())
                _queue[i]->start();
    }
    
}}}
