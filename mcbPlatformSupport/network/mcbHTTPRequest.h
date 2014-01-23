//
//  mcbHTTPRequest.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/5/14.
//
//

#ifndef __mcb__network__mcbHTTPRequest__
#define __mcb__network__mcbHTTPRequest__

#include <iostream>
#include <vector>
namespace mcb{namespace PlatformSupport{namespace network{
    struct HTTPRequest{
        typedef enum{TypeUnknown, TypeGet, TypePost} Type;
        std::string url;
        Type type;
        std::vector<std::string> headers;
        void setRequestString(const std::string & rData){requestData.assign(rData.begin(), rData.end());}
        std::vector<char> requestData;    /// used for POST
        bool operator == (const HTTPRequest & other) const{return url==other.url && type==other.type && headers==other.headers && requestData==other.requestData;}
    };
    static HTTPRequest HTTPRequestGET(const std::string & url){return HTTPRequest{url, HTTPRequest::TypeGet};}
    struct HTTPResponse{
        HTTPRequest request;
        int responseCode;
        std::vector<char> responseData;
        std::string responseString() const{return std::string(responseData.begin(),responseData.end());}
        bool succeed;
        std::string errorBuffer;   /// if _responseCode != 200, please read this to find the reason
    };
}}}
#endif /* defined(__mcb__network__mcbHTTPRequest__) */
