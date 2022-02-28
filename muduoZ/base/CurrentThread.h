#ifndef _MUDUOZ_BASE_CURRENTTHREAD_
#define _MUDUOZ_BASE_CURRENTTHREAD_

#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>


//用于存储每个线程的数据,以及数据的获取方法，便于logging
namespace CurrentThread {

__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "default";

//using namespace CurrentThread;
pid_t gettid() { return static_cast<pid_t>(::syscall(SYS_gettid)); }

//缓存该线程的线程id
void cacheTid(){
    if (t_cachedTid == 0) {
    t_cachedTid = gettid();
    t_tidStringLength =
        snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

//返回该线程的线程id
inline int tid() {
    if (__builtin_expect(t_cachedTid == 0, 0)) cacheTid();
    return t_cachedTid;//直接返回缓存的id
}

inline const char* tidString()  {
    return t_tidString;
}

inline int tidStringLength()  {
    return t_tidStringLength;
}

inline const char* name() { 
    return t_threadName; 
}

}

#endif