#ifndef _MUDUOZ_BASE_COUNTDOWNLATCH_
#define _MUDUOZ_BASE_COUNTDOWNLATCH_

#include"muduoZ/base/uncopyable.h"
#include"muduoZ/base/Mutex.h"
#include"muduoZ/base/Condition.h"

namespace muduoZ{

class CountDownLatch:uncopyable{
public:
    //cond_使用mutex_构造
    explicit CountDownLatch(int count):mutex_(), cond_(mutex_), count_(count){}

    void wait(){
        //把mutex锁在当前线程(父线程)
        MutexLockGuard lock(mutex_);
        //如子线程未运行完毕，wait，并让出锁，等待被唤醒；被唤醒后再判断count
        while(count_>0) cond_.wait();
    }

    void countDown(){
        //把mutex锁在当前线程(子线程)
        MutexLockGuard lock(mutex_);
        --count_;
        //如果子线程全部运行完毕了，通知父线程被唤醒
        if(count_==0) cond_.notifyAll();
    }
    
private:
    int count_;
    //mutable的锁，mutex需要尽可能接近其需要保护的对象，缩短临界区
    //假设拥有CountDownLatch的父类是const类型，其内部调用到这里的countDown，然后需要锁mutex
    //意味着，const对象调用非const方法，因此这里需要加上mutex限定符
    mutable Mutex mutex_;
    Condition cond_;

};

} // namespace muduoZ



#endif