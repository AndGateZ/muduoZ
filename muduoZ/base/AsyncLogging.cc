#include"muduoZ/base/AsyncLogging.h"
#include"muduoZ/base/LogFile.h"


namespace muduoZ{

AsyncLogging::AsyncLogging(const char* logFileName,size_t rollSize,int interval)
        :bufferCurrent_(new Buffer),
         bufferNext_(new Buffer),
         interval_(interval),
         logFileName_(logFileName),
         thread_(std::bind(&AsyncLogging::run,this),"Logging"),//bind函数run,this绑定
         mutex_(),
         cond_(mutex_),
         latch_(1),
         running_(false),
		 rollSize_(rollSize)
{
    bufferCurrent_->bZero();
    bufferNext_->bZero();
    bufferVector_.reserve(16);//分配空间修改尺寸
}

AsyncLogging::~AsyncLogging(){
    stop();
}

void AsyncLogging::start(){
    running_ = true;
    thread_.start();
    latch_.wait();//latch_将在扔进thread的run函数中countdown
}

void AsyncLogging::stop(){
    running_ = false;
    cond_.notifyAll();
    thread_.join();
}

//核心功能，接收来自前端的日志信息
void AsyncLogging::append(const char* msg,size_t len){
    MutexLockGuard lock(mutex_);//和后端避免冲突的锁
    //当前缓冲剩余空间足够
    if(bufferCurrent_->getRemainLen() >= static_cast<int>(len)) bufferCurrent_->append(msg,len);
    else{
        bufferVector_.push_back(std::move(bufferCurrent_));
        //备用缓冲未被使用
        if(bufferNext_) bufferCurrent_ = std::move(bufferNext_);
        //备用缓冲已被使用，new一个新的
        else {
            bufferCurrent_.reset(new Buffer);
            bufferCurrent_->resetCur();//写入位置重置
        }
        bufferCurrent_->append(msg,len);
    }
    cond_.notifyOne();//通知阻塞的子线程
}

//子线程运行
void AsyncLogging::run(){
    //初始化后端数据容器
    latch_.countDown();
    LogFile output(true,rollSize_,logFileName_);//初始化一个写入文件的LogFile
    BufferPtr bufferBack1(new Buffer);
    BufferPtr bufferBack2(new Buffer);
    bufferBack1->bZero();//初始化之后要记得重置
    bufferBack2->bZero();
    BufferVector bufferBackVec;
    bufferBackVec.reserve(16);//尺寸和前端一致
    while(running_){
        {
            MutexLockGuard lock(mutex_);//处理前端两个buffer以及buffer数组的锁
            
            if(bufferVector_.empty()) cond_.waitForSeconds(interval_);
            bufferBackVec.swap(bufferVector_);//直接交换前后的vector
            bufferBackVec.push_back(std::move(bufferCurrent_));//不用考虑currentBuffer有没有数据
            bufferCurrent_ = move(bufferBack1);
            bufferCurrent_->resetCur();//重置写入位置
            if(!bufferNext_) {
                bufferNext_ = move(bufferBack2);
                bufferNext_->resetCur();
            }
        }

        //此时bufferBack1一定空，bufferBack2可能空，需要提交数据到logfile，重置缓存
        if(bufferBackVec.size()>25) bufferBackVec.erase(bufferBackVec.begin() + 2, bufferBackVec.end());//数据太多的时候只保留两个
        
        //因为是uniqueptr，所以不能用for(BufferPtr bp:bufferBackVec)拷贝
        for(int i = 0;i<bufferBackVec.size();++i){
            output.output(bufferBackVec[i]->getData(),bufferBackVec[i]->getDataLen());//输入到后端logfile，实质传的是char*
        }

        if(bufferBackVec.size()>2) bufferBackVec.resize(2);

        if(!bufferBack1){
            bufferBack1 = move(bufferBackVec.back());
            bufferBackVec.pop_back();//只能理解为，bufferBack2里bufferBackVec调用back
            bufferBack1->resetCur();
        }
        
        if(!bufferBack2){
            bufferBack2 = move(bufferBackVec.back());
            bufferBackVec.pop_back();//只能理解为，bufferBack2里bufferBackVec调用back
            bufferBack2->resetCur();
        }
        bufferBackVec.clear();
        output.flush();
    }
    output.flush();
}

}