//
//  mcbNetworkTask.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/6/14.
//
//

#ifndef __SoundSurfer__mcbNetworkTask__
#define __SoundSurfer__mcbNetworkTask__

#include <iostream>
#include "mcbHTTPRequest.h"
namespace mcb{namespace PlatformSupport{namespace network{
    class NetworkTask;
    typedef std::shared_ptr<NetworkTask> pNetworkTask;
    class NetworkTask{
        struct callback{
            void * receiver=nullptr;
            callback();
            ~callback();
        }_callback;
        pNetworkTask _strongSelf=nullptr;
    public:
        typedef enum{StatusUndef, StatusDownloading, StatusCompleted, StatusFailed, StatusCancelled, StatusWriteFailed} Status;
        typedef std::function<void(float progress)> progress_handle_t;
        typedef std::function<void(Status status, const HTTPResponse & response)> completion_handle_t;
    protected:
        HTTPRequest _request;
        progress_handle_t _progressHandle=nullptr;
        completion_handle_t _completionHandle=nullptr;
        void setProgress(float progress);
        void retainSelf(const pNetworkTask & strongSelf);
        void releaseSelf();
    public:
        Status status() const{return _status;}
        bool isRunning() const{return _status==StatusDownloading;}
        bool hasStarted() const{return _status!=StatusUndef;}
        bool isFinished() const{return _status==StatusCompleted || _status==StatusFailed || _status==StatusCancelled || _status==StatusWriteFailed;}
        
        bool isEqual(const pNetworkTask & other) const;
        const HTTPRequest & request() const{return _request;}
        static pNetworkTask create(const HTTPRequest & request, const completion_handle_t & completion, const progress_handle_t & progress=nullptr);
        static void ping(const HTTPRequest & request, const completion_handle_t & completion, const progress_handle_t & progress=nullptr);
                
        virtual void start();
        virtual void cancel();
        
        
        static void setTimeoutForConnect(int value);
        static int timeoutForConnect();
        static void setTimeoutForRead(int value);
        static int timeoutForRead();
        
        virtual ~NetworkTask()=default;
    protected:

        Status _status=StatusUndef;
        NetworkTask()=default;//only to be used with the shared_ptr through create factory
    };
}}}
#endif /* defined(__SoundSurfer__mcbNetworkTask__) */
