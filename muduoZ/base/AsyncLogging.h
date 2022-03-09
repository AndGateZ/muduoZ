#ifndef _MUDUOZ_BASE_ASYNCLOGGING_
#define _MUDUOZ_BASE_ASYNCLOGGING_

#include<vector>
#include<string>
#include<memory>

#include"muduoZ/base/uncopyable.h"
#include"muduoZ/base/FixedBuffer.h"
#include"muduoZ/base/Thread.h"
#include"muduoZ/base/Mutex.h"
#include"muduoZ/base/Condition.h"
#include"muduoZ/base/CountDownLatch.h"

namespace muduoZ{

class AsyncLogging:uncopyable{
public:
    typedef muduoZ::FixedBuffer<bigBufferSize> Buffer;
    typedef std::unique_ptr<Buffer> BufferPtr;//使用unique_ptr
    typedef std::vector<BufferPtr> BufferVector;
    
    //默认参数
    explicit AsyncLogging(const char* logFileName,size_t rollSize,int interval = 3);
    ~AsyncLogging();
    void append(const char* msg,size_t len);
    void start();//thread.start
    void stop();
    void run();//子线程运行，与线程绑定

private:
    BufferPtr bufferCurrent_;//前台缓冲1,当前使用的缓冲,指针
    BufferPtr bufferNext_;//前台缓冲2，后备缓冲
    BufferVector bufferVector_;//前台存储缓冲数组
    size_t interval_;//flush缓冲的间隔时间
    std::string logFileName_;//最终的log文件名
    Thread thread_;//拥有自己的线程，线程用于循环运行日志收取
    Mutex mutex_;
    Condition cond_;
    CountDownLatch latch_;//子线程倒计数器
    bool running_;//指示是否正在运行
	size_t rollSize_;
};

}


#endif