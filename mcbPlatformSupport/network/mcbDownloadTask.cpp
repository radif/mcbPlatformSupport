//
//  mcbDownloadTask.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/6/14.
//
//

#include "mcbDownloadTask.h"
namespace mcb{namespace PlatformSupport{namespace network{
    
    void DownloadTask::download(const HTTPRequest & request, const std::string & saveToPath, const completion_handle_t & completion, const progress_handle_t & progress){
        pDownloadTask task(create(request, saveToPath, completion, progress));
        task->retainSelf(task);
        task->start();
    }
    
    pDownloadTask DownloadTask::create(const HTTPRequest & request, const std::string & saveToPath, const completion_handle_t & completion, const progress_handle_t & progress){
        pDownloadTask retVal(pDownloadTask(new DownloadTask));
        retVal->_request=request;
        retVal->_saveToPath=saveToPath;
        std::weak_ptr<DownloadTask> weakTask(retVal);
        //removing file first
        std::remove(saveToPath.c_str());
        retVal->_completionHandle=[=](Status status, const HTTPResponse & response){
            //write to file
            if (response.succeed) {
                pDownloadTask capturedWeakTask(weakTask.lock());
                if (capturedWeakTask) {
                    FILE *f(fopen(capturedWeakTask->_saveToPath.c_str(), "wb"));
                    if (f){
                        fwrite((unsigned char*)response.responseData.data(), response.responseData.size(), 1, f);
                        fclose(f);
                    }else{
                        status=StatusWriteFailed;
                        capturedWeakTask->_status=status;
                    }
                }
            }
            if (completion)
                completion(status, response);
            
        };
        retVal->_progressHandle=progress;
        return retVal;
    }
}}}
