#include "ThreadPool.h"

void ThreadPool::ThreadWorker::operator()(){
    std::function<void()> func;
    bool dequeued;
    while(threadPool->shutDownFlag==false){
        {
            std::unique_lock<std::mutex> lock(threadPool->mute);
            //这里如果用while，这个锁不会被释放，即可能一直由一个线程获取任务,同时导致无法shutDown,
            //线程是轮流获得锁的，获得锁=获得一次查询任务的机会
            if(threadPool->que.empty()) threadPool->cv.wait(lock);
            //获得任务信息
            dequeued = threadPool->que.dequeue(func);
            //离开作用域，释放锁
        }
        //执行任务
        if(dequeued==true) func();
    }
}


void ThreadPool::init(){
    for(size_t i = 0;i<threads.size();i++){
        //这里给thread初始化的函数是，可调用的对象
        threads[i] = std::thread(ThreadWorker(this,i));
    }
}

void ThreadPool::shutDown(){
    shutDownFlag = true;
    //唤醒所有线程
    //他们会执行完当前分配的任务，然后剩下的任务不做了
    cv.notify_all();
    for(int i = 0;i<threads.size();i++){
        if(threads[i].joinable()) threads[i].join();
    }
}

