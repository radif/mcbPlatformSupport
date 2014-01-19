//
//  mcbDownloadTask.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/6/14.
//
//

#include "mcbDownloadTask.h"
#include "mcbPlatformSupportFunctions.h"
#include "mcbMainThreadCaller.h"

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
        retVal->_completionHandle=[=](Status status, const HTTPResponse & response){
            //write to file
            if (response.succeed) {
                pDownloadTask capturedWeakTask(weakTask.lock());
                if (capturedWeakTask) {
                    //removing file first
                    Functions::removeFile(capturedWeakTask->_saveToPath);
                    //write new content
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
            //emptying the data to prevent copying in stack
            decltype(response.responseData) emptyData;
            HTTPResponse responseWithoutData{response.request, response.responseCode, emptyData, response.succeed, response.errorBuffer};
            
            call_on_main_thread([=](){
                if (completion)
                    completion(status, responseWithoutData);
            });
        };
        retVal->_progressHandle=progress;
        return retVal;
    }
}}}
