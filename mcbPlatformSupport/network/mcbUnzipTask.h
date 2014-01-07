//
//  mcbUnzipTask.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/6/14.
//
//

#ifndef __SoundSurfer__mcbUnzipTask__
#define __SoundSurfer__mcbUnzipTask__

#include <iostream>
namespace mcb{namespace PlatformSupport{namespace network{
    class UnzipTask;
    typedef std::shared_ptr<UnzipTask> pUnzipTask;
    class UnzipTask{
        pUnzipTask _strongSelf=nullptr;
        void setProgress(float progress);
    public:
        typedef enum{StatusUndef, StatusUnzipping, StatusCompleted, StatusFailed} Status;
        typedef std::function<void(float progress)> progress_handle_t;
        typedef std::function<void(Status status)> completion_handle_t;
        typedef std::function<void(std::string path, bool success)> completed_each_t;

    private:
        Status _status=StatusUndef;
        progress_handle_t _progressHandle=nullptr;
        completion_handle_t _completionHandle=nullptr;
        completed_each_t _completedEachHandle=nullptr;
        std::string _archivedFile;
        std::string _destinationPath;
        
        void retainSelf(const pUnzipTask & strongSelf);
        void releaseSelf();
    public:
        Status status() const{return _status;}
        bool isRunning() const{return _status==StatusUnzipping;}
        bool hasStarted() const{return _status!=StatusUndef;}
        bool isFinished() const{return _status==StatusCompleted || _status==StatusFailed;}
        static pUnzipTask create(const std::string & archivedFile, const std::string & destinationPath, const completion_handle_t & completion, const progress_handle_t & progress=nullptr, const completed_each_t & completedEach=nullptr);
        static void unzip(const std::string & archivedFile, const std::string & destinationPath, const completion_handle_t & completion, const progress_handle_t & progress=nullptr, const completed_each_t & completedEach=nullptr);
        virtual ~UnzipTask()=default;
        void start();
        
        std::string archivedFile() const{return _archivedFile;}
        std::string destinationPath() const{return _destinationPath;}
        
    protected:
        UnzipTask()=default;
    };
}}}
#endif /* defined(__SoundSurfer__mcbUnzipTask__) */
