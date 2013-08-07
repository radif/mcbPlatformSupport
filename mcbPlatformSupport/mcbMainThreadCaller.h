//
//  mcbMainThreadCaller.h
//  mcb
//
//  Created by Radif Sharafullin on 6/8/13.
//
//

#ifndef __mcb__MainThreadCaller__
#define __mcb__MainThreadCaller__

#include <iostream>

/*
 To be used with std::async
 useful for working threads callbacks
 
 std::async(std::launch::async, [](){
 
     std::cout << "Background Thread ID:"<<std::this_thread::get_id()<<std::endl;
 
     PlatformSupport::call_on_main_thread([](){
         std::cout << "Main Thread ID:"<<std::this_thread::get_id()<<std::endl;
     });
 });
 
 */

namespace mcb{namespace PlatformSupport{
    void call_on_main_thread(std::function<void()> mainThreadFunction, bool waitUntilDone=false);
}}

#endif /* defined(__mcb__MainThreadCaller__) */
