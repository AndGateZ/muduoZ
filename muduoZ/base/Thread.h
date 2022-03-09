#ifndef _MUDUOZ_BASE_THREAD_
#define _MUDUOZ_BASE_THREAD_

#include <string>
#include <functional>
#include <pthread.h>

#include"muduoZ/base/uncopyable.h"
#include"muduoZ/base/CountDownLatch.h"

typedef std::function<void()> Function;

namespace muduoZ{
class Thread:uncopyable{
public:
    //参数是const 引用
    explicit Thread(const Function &func,const std::string &name = std::string("Thread"));
    //线程结束时要判断，如果曾经运行过join，需要将子线程分离
    ~Thread();

    void start();//线程启动,在本线程运行，调用pthread_create，将startThread放到子线程运行
    int join();//线程同步，通过返回值来判断是否同步成功
    static void* startThread(void* obj);
    bool running() const {return running_;}
    const std::string& name() const {return name_;}


private:
    Function func_;
    std::string name_;
    bool running_;
    bool joined_;
    CountDownLatch latch_;
    pthread_t pthreadId_;//子线程标识符，指向线程创建成功时线程id所在的内存单元，进程内唯一，不同进程内可能相同
    pid_t tid_;//子线程id，内核提供的进程唯一的标识，linux下线程都是进程模拟的，全局唯一，不同进程内也不同
};

struct ThreadData{
    Function func_;
    std::string name_;
    pid_t* tid_;//自己的线程号，父线程传入指针，需要自己通过CurrentThread来获取tid
    CountDownLatch* latch_;//这里使用指针类型，如果用引用就随着ThreadData销毁而销毁了

    ThreadData(const Function &func,const std::string name,pid_t* tid,CountDownLatch* latchA);

    void run();//在本线程中工作

};

}

#endif