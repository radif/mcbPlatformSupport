//
//  mcbBase64.h
//  crypto
//
//  Created by Radif Sharafullin on 1/23/14.
//
//

#ifndef __mcb__crypto__mcbBase64__
#define __mcb__crypto__mcbBase64__

//usage:
/*
 std::string original("Hello World!");
 
 auto s(mcb::PlatformSupport::crypto::base64EncodeS(original));
 auto ret(mcb::PlatformSupport::crypto::base64DecodeS(s));
 
 assert(original==ret);
 
 std::cout <<s<<std::endl;
 std::cout <<ret<<std::endl;
 */

#include <vector>
#include <string>
namespace mcb{namespace PlatformSupport{namespace crypto{
    typedef unsigned char byte_t;
    typedef std::vector<byte_t> data_t;
    
    std::string base64Encode(byte_t const* buf, unsigned int bufLen);
    static inline std::string base64Encode(const data_t & data){return base64Encode(&data.front(), data.size());}
    static inline std::string base64EncodeS(const std::string & s){return base64Encode(&data_t(s.begin(), s.end()).front() ,s.size());}
    
    data_t base64Decode(const std::string & encodedString);
    static inline std::string base64DecodeS(const std::string & encodedString){
        auto data(base64Decode(encodedString));
        return std::string(data.begin(), data.end());
    }
}}}
#endif /* defined(__mcb__crypto__mcbBase64__) */
