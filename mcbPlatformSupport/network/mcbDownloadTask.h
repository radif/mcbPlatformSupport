//
//  mcbDownloadTask.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/6/14.
//
//

#ifndef __SoundSurfer__mcbDownloadTask__
#define __SoundSurfer__mcbDownloadTask__

#include "mcbNetworkTask.h"
namespace mcb{namespace PlatformSupport{namespace network{
    class DownloadTask;
    typedef std::shared_ptr<DownloadTask> pDownloadTask;
    class DownloadTask : public NetworkTask{
        std::string _saveToPath;
    public:
        
        //completion will not have data, since the data will be written to a file
        
        static pDownloadTask create(const HTTPRequest & request, const std::string & saveToPath, const completion_handle_t & completion, const progress_handle_t & progress=nullptr);
                
        static void download(const HTTPRequest & request, const std::string & saveToPath, const completion_handle_t & completion, const progress_handle_t & progress=nullptr);
        
        
        std::string saveToPath() const{return _saveToPath;}
        virtual ~DownloadTask()=default;
    protected:
        DownloadTask()=default;//only to be used with the shared_ptr through create factory
    };
}}}
#endif /* defined(__SoundSurfer__mcbDownloadTask__) */
