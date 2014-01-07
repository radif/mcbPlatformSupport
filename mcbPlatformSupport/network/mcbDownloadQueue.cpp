//
//  mcbDownloadQueue.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/4/14.
//
//

#include "mcbDownloadQueue.h"
namespace mcb{namespace PlatformSupport{namespace network{
    void DownloadQueue::init(){
        
    }
    void DownloadQueue::setMaxConcurrentDownloads(int maxConcurrentDownloads){
        _maxConcurrentDownloads=maxConcurrentDownloads;
    }
    bool DownloadQueue::isDownloading(const HTTPRequest & request) const{
        for (const auto & task: _queue)
            if (task->request()==request)
                return true;
        return false;
    }
    bool DownloadQueue::enqueueDownload(const HTTPRequest & request, const std::string & saveToPath, const DownloadTask::completion_handle_t & completion, const DownloadTask::progress_handle_t & progress){
        if (isDownloading(request))
            return false;
        
        pDownloadTask task(DownloadTask::create(request, saveToPath, [=](NetworkTask::Status status, const HTTPResponse & response){
            if (completion)
                completion(status, response);
            //remove from queue
            circulate();
        }, progress));
        _queue.emplace_back(task);
        
        circulate();
        return true;
    }
    void DownloadQueue::circulate(){
        //remove unused
        decltype(_queue) filtered;
        for (const auto & task : _queue)
            if (!task->isFinished())
                filtered.emplace_back(task);
            
        std::swap(_queue, filtered);
        
        
        //launch up to max concurent
        int count(fminf(_queue.size(), _maxConcurrentDownloads));
        for (int i(0); i<count; ++i)
            if (!_queue.at(i)->hasStarted())
                _queue[i]->start();
    }
    void DownloadQueue::cancelDownload(const HTTPRequest & request){
        decltype(_queue)::iterator it;
        for (it=_queue.begin(); it!=_queue.end(); ++it) {
            auto & task(*it);
            if (task->request()==request){
                task->cancel();
                break;
            }
        }
        if (it!=_queue.end())
            _queue.erase(it);
    }
    void DownloadQueue::cancelAllDownloads(){
        for (const auto task : _queue)
            task->cancel();
        //clearing the queue
        _queue.clear();
    }
    
    void DownloadQueue::setTimeoutForConnect(int value){
        NetworkTask::setTimeoutForConnect(value);
    }
    int DownloadQueue::timeoutForConnect(){
        return NetworkTask::timeoutForConnect();
    }
    void DownloadQueue::setTimeoutForRead(int value){
        NetworkTask::setTimeoutForRead(value);
    }
    int DownloadQueue::timeoutForRead(){
        return NetworkTask::timeoutForConnect();
    }
}}}
