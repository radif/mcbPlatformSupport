//
//  mcbNetworkTask.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/6/14.
//
//

#include "mcbNetworkTask.h"
#include "cocos-ext.h"
using namespace cocos2d;
using namespace cocos2d::extension;

namespace mcb{namespace PlatformSupport{namespace network{
    
    //responder
    class NetworkTaskCallbackReceiver : public CCObject{
        virtual ~NetworkTaskCallbackReceiver()=default;
    public:
        std::function<void(CCHttpResponse *response)> callbackHandle=nullptr;
        void dataReceived(CCNode *pSender, void *pData){
            if (callbackHandle)
                callbackHandle(static_cast<CCHttpResponse *>(pData));
        }
    };

    
    NetworkTask::callback::callback(){
        receiver=new NetworkTaskCallbackReceiver;
    }
    //cleanup
    NetworkTask::callback::~callback(){
        static_cast<NetworkTaskCallbackReceiver *>(receiver)->callbackHandle=nullptr;
        static_cast<NetworkTaskCallbackReceiver *>(receiver)->release();
    }
    
    void NetworkTask::ping(const HTTPRequest & request, const completion_handle_t & completion, const progress_handle_t & progress){
        pNetworkTask task(create(request, completion, progress));
        task->retainSelf(task);
        task->start();
    }
    
    pNetworkTask NetworkTask::create(const HTTPRequest & request, const completion_handle_t & completion, const progress_handle_t & progress){
        pNetworkTask retVal(pNetworkTask(new NetworkTask));
        retVal->_request=request;
        retVal->_completionHandle=completion;
        retVal->_progressHandle=progress;
        return retVal;
    }
    void NetworkTask::start(){
        // Fetch from the server if forced or there is no cached file
        setProgress(0.f);
        _status=StatusDownloading;
        
        CCHttpRequest *req = new CCHttpRequest();
        req->setUrl(_request.url.c_str());
        
        CCHttpRequest::HttpRequestType requestType(CCHttpRequest::kHttpUnkown);
        switch (_request.type) {
            case HTTPRequest::TypeGet:
                requestType=CCHttpRequest::kHttpGet;
                break;
            case HTTPRequest::TypePost:
                requestType=CCHttpRequest::kHttpPost;
                break;
            default:
                break;
        }
        req->setRequestType(requestType);
        req->setHeaders(_request.headers);
       
        req->setRequestData(&_request.requestData.front(), _request.requestData.size());
        req->setResponseCallback(static_cast<NetworkTaskCallbackReceiver *>(_callback.receiver), callfuncND_selector(NetworkTaskCallbackReceiver::dataReceived));
        
        
        static_cast<NetworkTaskCallbackReceiver *>(_callback.receiver)->callbackHandle=[=](CCHttpResponse *response){
            _status=response->isSucceed()?StatusCompleted:StatusCancelled;
            setProgress(1.f);
            if (_completionHandle)
                _completionHandle(_status, HTTPResponse{_request, response->getResponseCode(),std::move(*response->getResponseData()), response->isSucceed(), response->getErrorBuffer()});
            releaseSelf();
        };
        
        CCHttpClient::getInstance()->send(req);
        req->release();
    }
    void NetworkTask::setProgress(float progress){
        if (_progressHandle)
            _progressHandle(progress);
    }
    void NetworkTask::cancel(){
        if (!isRunning())
            return;
        
        //TODO: cancel actual download
        
        _status=StatusCancelled;
        if (_completionHandle)
            _completionHandle(_status, HTTPResponse{_request});
        releaseSelf();
    }
    void NetworkTask::releaseSelf(){
        _strongSelf=nullptr;
    }
    void NetworkTask::retainSelf(const pNetworkTask & strongSelf){
        _strongSelf=strongSelf;
    }
    void NetworkTask::setTimeoutForConnect(int value){
        CCHttpClient::getInstance()->setTimeoutForConnect(value);
    }
    int NetworkTask::timeoutForConnect(){
        return CCHttpClient::getInstance()->getTimeoutForConnect();
    }
    void NetworkTask::setTimeoutForRead(int value){
        CCHttpClient::getInstance()->setTimeoutForRead(value);
    }
    int NetworkTask::timeoutForRead(){
        return CCHttpClient::getInstance()->getTimeoutForRead();
    }
    
    bool NetworkTask::isEqual(const pNetworkTask & other) const{
        return _request==other->_request && _status==other->_status;
    }

}}}