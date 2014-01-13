//
//  mcbBundleFetcher.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/12/14.
//
//

#ifndef __SoundSurfer__mcbBundleFetcher__
#define __SoundSurfer__mcbBundleFetcher__

#include "mcbFactory.h"
#include "mcbHTTPRequest.h"
#include "mcbBundle.h"

namespace mcb{namespace PlatformSupport{namespace network{
    typedef const std::vector<pBundle> pBundles;
    class BundleFetcher : public SingletonFactory<BundleFetcher>{
        std::set<std::string> _bundlesInProcess;
        pBundles _bundles;
    public:
        //this will not have effect, once the initial connection with server is established. tokens injected onto mcbPath
        void initPreshippedDataWithPath(const std::string path);
        void synchronizeWithServer();//fetch metadata, update or download
        bool isSynchronizingWithServer() const;
        pBundles & bundles() const;

        //once bundle is downloaded, new tokens will be injected to mcbPath
        void downloadAndUnzipBundle(const HTTPRequest & request, const std::string & bundlesDirectory, const std::function<void(const std::string & bundlePath, bool success)> & completion, const std::string & bundleID="", const std::function<void(float progress)> & progress=nullptr);
        

    protected:
        virtual void init() override;
    };
}}}

#endif /* defined(__SoundSurfer__mcbBundleFetcher__) */
