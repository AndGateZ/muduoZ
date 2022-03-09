#include <assert.h>
#include"muduoZ/base/Thread.h"
#include"muduoZ/base/CurrentThread.h"

namespace muduoZ{

//这个函数可以是类的静态成员函数，或者不属于类的普通函数
void* Thread::startThread(void* obj){
    ThreadData* tData = static_cast<ThreadData*> (obj);
    tData->run();
    delete tData;//回收数据
    return NULL;
}

Thread::Thread(const Function &func,const std::string &name)
    :func_(func),name_(name),running_(false),joined_(false),latch_(1),pthreadId_(0),tid_(0){}

Thread::~Thread(){
    if (running_ && !joined_) pthread_detach(pthreadId_);
}

void Thread::start(){
    assert(!running_);
    running_ = true;
    ThreadData* tData = new ThreadData(func_,name_,&tid_,&latch_);
    if(pthread_create(&pthreadId_,NULL,Thread::startThread,tData)){
        delete tData;
        running_ = false;//启动失败
    }
    else{//启动成功
        latch_.wait();
        assert(tid_ > 0);//在另一个线程tid_被赋值
    }
}

int Thread::join(){
    assert(running_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_,NULL);
}


ThreadData::ThreadData(const Function &func,const std::string name,pid_t* tid,CountDownLatch* latchA)
        :func_(func),name_(name),tid_(tid),latch_(latchA){}

void ThreadData::run(){
    //在这个线程中获取CurrentThread的信息，用于log等操作
    *tid_ = CurrentThread::tid();
    tid_ = NULL;
    CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
    prctl(PR_SET_NAME, CurrentThread::t_threadName);

    latch_->countDown();
    latch_ = NULL;
    func_();
    
    CurrentThread::t_threadName = "finished";
}

}