//
//  mcbUnzipTask.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/6/14.
//
//

#include "mcbUnzipTask.h"
#include "mcbMainThreadCaller.h"
#include <future>
#include "support/zip_support/unzip.h"
#include "mcbPlatformSupportFunctions.h"
#include <sys/stat.h>

using namespace cocos2d;
namespace mcb{namespace PlatformSupport{namespace network{
    struct FutureHolder {std::future<void> future;};

    pUnzipTask UnzipTask::create(const std::string & archivedFile, const std::string & destinationPath, const completion_handle_t & completion, const progress_handle_t & progress, const completed_each_t & completedEach){
        pUnzipTask retVal(pUnzipTask(new UnzipTask));
        retVal->_completionHandle=completion;
        retVal->_progressHandle=progress;
        retVal->_completedEachHandle=completedEach;
        retVal->_archivedFile=archivedFile;
        retVal->_destinationPath=destinationPath;
        return retVal;
    }
    void UnzipTask::unzip(const std::string & archivedFile, const std::string & destinationPath, const completion_handle_t & completion, const progress_handle_t & progress, const completed_each_t & completedEach){
        pUnzipTask task(create(archivedFile, destinationPath, completion, progress, completedEach));
        task->retainSelf(task);
        task->start();
    }
    void UnzipTask::start(){
        _status=StatusUnzipping;
        
        FutureHolder * f(new FutureHolder);
        
        setProgress(0.f);
        f->future=std::async(std::launch::async, [=](){
        
            auto callbackL([=](Status status){
                call_on_main_thread([=]{
                    _status=status;
                    releaseSelf();
                    if (_completionHandle)
                        _completionHandle(_status);
                    delete f;
                });
            });
            
            auto eachCallbackL([=](const std::string & path, bool success){
                if (_completedEachHandle)
                    call_on_main_thread([=]{
                        _completedEachHandle(path, success);
                    });
            });
            
            unsigned long ulSize;
            unsigned char *pBuffer(nullptr);
            
            //background work
            unzFile pFile(unzOpen(_archivedFile.c_str()));
            if (pFile){
                int iStatus(unzGoToFirstFile(pFile));
                while (iStatus == UNZ_OK){
                    char szFilePath[260];
                    unz_file_info fileInfo;
                    iStatus = unzGetCurrentFileInfo(pFile, &fileInfo, szFilePath, sizeof(szFilePath), NULL, 0, NULL, 0);
                    if (iStatus != UNZ_OK){
                        break;
                    }
                    iStatus = unzOpenCurrentFile(pFile);
                    if (iStatus != UNZ_OK){
                        break;
                    }
                    pBuffer=new unsigned char[fileInfo.uncompressed_size];
                    ulSize=unzReadCurrentFile(pFile, pBuffer, fileInfo.uncompressed_size);
                    unzCloseCurrentFile(pFile);
                    std::string filePath(szFilePath);

                    bool isDirectory(false);
                    if (!filePath.empty())
                        isDirectory=filePath.back()=='/';
                    std::string path(Functions::stringByAppendingPathComponent(_destinationPath, filePath));
                    if (isDirectory) {
                        int result(mkdir(path.c_str(),0777));
                        eachCallbackL(path+"/", result==0);
                    }else{
                        FILE *pFHandle(fopen(path.c_str(), "wb"));
                        if (pFHandle){
                            fwrite(pBuffer, fileInfo.uncompressed_size, 1, pFHandle);
                            fclose(pFHandle);
                            eachCallbackL(path, true);
                        }else
                            eachCallbackL(path, false);
                        
                        }
                    delete [] pBuffer;
                    iStatus = unzGoToNextFile(pFile);
                }
                unzClose(pFile);
                callbackL(StatusCompleted);
                setProgress(1.f);
            }else
                callbackL(StatusFailed);
            
            
        });
        
    }
    void UnzipTask::retainSelf(const pUnzipTask & strongSelf){
        _strongSelf=strongSelf;
    }
    void UnzipTask::releaseSelf(){
        _strongSelf=nullptr;
    }

    void UnzipTask::setProgress(float progress){
        if (_progressHandle)
            _progressHandle(progress);
    }
}}}