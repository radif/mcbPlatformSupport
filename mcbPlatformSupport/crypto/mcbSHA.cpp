//
//  mcbSHA.cpp
//  sha
//
//  Created by Radif Sharafullin on 1/20/14.
//  Copyright (c) 2014 Radif Sharafullin. All rights reserved.
//

#include "mcbSHA.h"
#include <vector>

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error this code has only been tested on little endian machines
#endif


#define shr64(x,n) ((x & 0xFFFFFFFFFFFFFFFFULL) >> n)
#define rotr64(x,n) (shr64(x,n) | (x << (64 - n)))

#define shr32(x,n) ((x & 0xFFFFFFFF) >> n)
#define rotr32(x,n) (shr32(x,n) | (x << (32 - n)))

namespace mcb{namespace PlatformSupport{namespace crypto{
    typedef unsigned int uint;
    typedef unsigned long long ullint;
    
    static inline std::string fromDecimal(ullint n, const ullint b, const uint length){
        static const std::string chars("0123456789abcdefghijklmnopqrstuvwxyz");
        std::string result;
        for (int pass(0); pass<length; ++pass){
            result=chars.at(n%b)+result;
            n/=b;
        }
        return result;
    }
    namespace sha512helpers{
        
        static const ullint K[80] = {
            0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
            0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
            0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
            0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
            0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
            0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
            0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
            0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
            0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
            0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
            0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
            0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
            0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
            0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
            0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
            0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
            0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
            0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
            0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
            0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
            0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
            0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
            0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
            0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
            0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
            0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
            0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
            0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
            0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
            0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
            0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
            0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
            0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
            0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
            0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
            0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
            0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
            0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
            0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
            0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
        };
        
        
        static void makeblock(std::vector<ullint>& ret, const std::string & p_msg){
            ullint cur(0);
            int ind(0);
            for(uint i(0); i<p_msg.size(); ++i){
                cur = (cur<<8) | (unsigned char)p_msg[i];
                if(i%8==7){
                    ret.at(ind++)=cur;
                    cur=0;
                }
            }
        }
        struct Block{
            std::vector<ullint> msg;
            Block():msg(16, 0){}
            Block(const std::string & p_msg):msg(16, 0){
                makeblock(msg, p_msg);
            }
        };
        
        
        static void split(std::vector<Block>& blks, const std::string& msg){
            for(uint i(0); i<msg.size(); i+=128)
                makeblock(blks[i/128].msg, msg.substr(i, 128));
        }
        
        static std::string mynum(ullint x){
            std::string ret;
            for(uint i(0); i<8; ++i)
                ret+=char(0);
            
            for(uint i(8); i>=1; --i)	//big endian machine assumed ???
                ret += ((char*)(&x))[i-1];
            return ret;
        }
        
        
        
        static inline ullint ch(ullint x, ullint y, ullint z){
            return (x&y) ^ (~x&z);
        }
        
        static inline ullint maj(ullint x, ullint y, ullint z){
            return (x&y) ^ (y&z) ^ (z&x);
        }
        
        static inline ullint fn0(ullint x){
            return rotr64(x, 28) ^ rotr64(x, 34) ^ rotr64(x, 39);
        }
        
        static inline ullint fn1(ullint x){
            return rotr64(x, 14) ^ rotr64(x, 18) ^ rotr64(x, 41);
        }
        
        static inline ullint sigma0(ullint x){
            return rotr64(x, 1) ^ rotr64(x, 8) ^ shr64(x, 7);
        }
        
        static inline ullint sigma1(ullint x){
            return rotr64(x, 19) ^ rotr64(x, 61) ^ shr64(x, 6);
        }
        
        
        
    }
    std::string sha512(const std::string & str){
        using namespace sha512helpers;
        
        std::string msg_arr(str);
        
        msg_arr += (char)(1<<7);
        uint cur_len((uint)str.size()*8 + 8);
        uint reqd_len((uint)((str.size()*8)/1024+1) *1024);
        uint pad_len(reqd_len - cur_len - 128);
        
        std::string pad(pad_len/8, char(0));
        msg_arr+=pad;
        
        std::string len_str(mynum(str.size()*8));
        msg_arr+=len_str;
        
        uint num_blk((uint)msg_arr.size()*8/1024);
        std::vector<Block> M(num_blk, Block());
        split(M, msg_arr);
        
        ullint H[]={
            0x6a09e667f3bcc908ULL,
            0xbb67ae8584caa73bULL,
            0x3c6ef372fe94f82bULL,
            0xa54ff53a5f1d36f1ULL,
            0x510e527fade682d1ULL,
            0x9b05688c2b3e6c1fULL,
            0x1f83d9abfb41bd6bULL,
            0x5be0cd19137e2179ULL
        };
        
        for(uint i(0); i<num_blk; ++i){
            std::vector<ullint> W(80, 0);
            for(uint t(0); t<16; ++t)
                W[t] = M[i].msg[t];
            
            
            for(uint t(16); t<80; ++t)
                W[t] = sigma1(W[t-2]) + W[t-7] + sigma0(W[t-15]) + W[t-16];
            
            ullint work[8];
            for(uint i(0); i<8; ++i)
                work[i] = H[i];
            
            for(uint t(0); t<80; ++t){
                ullint t1, t2;
                t1 = work[7] + fn1(work[4]) + ch(work[4], work[5], work[6]) + K[t] + W[t];
                t2 = fn0(work[0]) + maj(work[0], work[1], work[2]);
                work[7] = work[6];
                work[6] = work[5];
                work[5] = work[4];
                work[4] = work[3] + t1;
                work[3] = work[2];
                work[2] = work[1];
                work[1] = work[0];
                work[0] = t1 + t2;
                
            }
            
            for(uint i(0); i<8; ++i)
                H[i] = work[i] + H[i];
        }
        
        std::string retVal;
        for(uint i(0); i<8; ++i)
            retVal+=fromDecimal(H[i], 16, 16);
        return retVal;
    }
    
    
    namespace sha256helpers{
        static const uint K[]=
        {   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
        
        static void makeblock(std::vector<uint>& ret, const std::string  & p_msg){
            uint cur(0);
            int ind(0);
            for(uint i(0); i<p_msg.size(); ++i){
                cur = (cur<<8) | (unsigned char)p_msg[i];
                if(i%4==3){
                    ret.at(ind++)=cur;
                    cur=0;
                }
            }
        }
        
        struct Block{
            std::vector<uint> msg;
            Block():msg(16, 0) {}
            Block(std::string p_msg):msg(16, 0){
                makeblock(msg, p_msg);
            }
            
        };
        
        
        static void split(std::vector<Block>& blks, const std::string& msg){
            for(uint i(0); i<msg.size(); i+=64){
                try{
                    makeblock(blks[i/64].msg, msg.substr(i, 64));
                }
                catch(...){
                    
                }
            }
        }
        
        
        static std::string mynum(uint x){
            std::string ret;
            for(uint i(0); i<4; ++i)
                ret+=char(0);
            for(uint i(4); i>=1; --i)	//big endian machine assumed ???
                ret += ((char*)(&x))[i-1];
            
            return ret;
        }
        
        
        
        
        static inline uint ch(uint x, uint y, uint z){
            return (x&y) ^ (~x&z);
        }
        
        static inline uint maj(uint x, uint y, uint z){
            return (x&y) ^ (y&z) ^ (z&x);
        }
        
        static inline uint fn0(uint x){
            return rotr32(x, 2) ^ rotr32(x, 13) ^ rotr32(x, 22);
        }
        
        static inline uint fn1(uint x){
            return rotr32(x, 6) ^ rotr32(x, 11) ^ rotr32(x, 25);
        }
        
        static inline uint sigma0(uint x){
            return rotr32(x, 7) ^ rotr32(x, 18) ^ shr32(x, 3);
        }
        
        static inline uint sigma1(uint x){
            return rotr32(x, 17) ^ rotr32(x, 19) ^ shr32(x, 10);
        }
        
    }
    
    std::string sha256(const std::string & str){
        using namespace sha256helpers;
        std::string msg_arr(str);
        msg_arr+=(char)(1<<7);
        uint cur_len((uint)str.size()*8 + 8);
        uint reqd_len((uint)((str.size()*8)/512+1) *512);
        uint pad_len((uint)reqd_len - cur_len - 64);
        
        std::string pad(pad_len/8, char(0));
        msg_arr += pad;
        std::string len_str(mynum((uint)str.size()*8));
        msg_arr = msg_arr + len_str;
        
        uint num_blk((uint)msg_arr.size()*8/512);
        std::vector<Block> M(num_blk, Block());
        split(M, msg_arr);
        
		uint H[]={0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f,  	0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
		
		for(uint i(0); i<num_blk; ++i){
			std::vector<uint> W(64, 0);
			for(uint t(0); t<16; ++t)
				W[t] = M[i].msg[t];
			
			
			for(uint t(16); t<64; ++t)
				W[t] = sigma1(W[t-2]) + W[t-7] + sigma0(W[t-15]) + W[t-16];
			
			uint work[8];
			for(uint i(0); i<8; ++i)
                work[i] = H[i];
            
			for(uint t(0); t<64; ++t){
				uint t1, t2;
				t1 = work[7] + fn1(work[4]) + ch(work[4], work[5], work[6]) + K[t] + W[t];
				t2 = fn0(work[0]) + maj(work[0], work[1], work[2]);
				work[7] = work[6];
				work[6] = work[5];
				work[5] = work[4];
				work[4] = work[3] + t1;
				work[3] = work[2];
				work[2] = work[1];
				work[1] = work[0];
				work[0] = t1 + t2;
                
			}
			
			for(uint i(0); i<8; ++i)
                H[i] = work[i] + H[i];
		}
		
        
        
        std::string retVal;
        for(uint i(0); i<8; ++i)
            retVal+=fromDecimal(H[i], 16, 8);
        return retVal;
    }
    
}}}
