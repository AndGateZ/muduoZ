#ifndef _MUDUOZ_BASE_MUTEX_
#define _MUDUOZ_BASE_MUTEX_

#include<pthread.h>

#include"muduoZ/base/uncopyable.h"

namespace muduoZ{

class Mutex:uncopyable{
public:
    Mutex(){pthread_mutex_init(&mutex_,NULL);}
    ~Mutex(){
        //先锁住再销毁
        pthread_mutex_lock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }

    void lock(){ pthread_mutex_lock(&mutex_); }
    void unlock(){ pthread_mutex_unlock(&mutex_); }
    pthread_mutex_t *getMutex(){return &mutex_;}

private:
    pthread_mutex_t mutex_;

//声明友员，使Condition可以访问mutex_，调用底层api对其操作
friend class Condition;

};

//自动绑定并加锁，在析构函数中解锁
class MutexLockGuard:uncopyable{
public:
    //显式构造，不允许隐式转换
    explicit  MutexLockGuard(Mutex &mutexA):mutex(mutexA){mutex.lock();}
    ~MutexLockGuard(){mutex.unlock();}

private:
    //用引用类型的mutex即可
    Mutex &mutex;

};

} // namespace muduoZ




#endif