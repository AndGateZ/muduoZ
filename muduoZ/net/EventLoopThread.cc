
#include "muduoZ/net/EventLoopThread.h"

namespace muduoZ{

namespace net{

EventLoopThread::EventLoopThread(const ThreadInitCallBack &cb,const std::string &name)
	:loop_(NULL),
	thread_(std::bind(&EventLoopThread::threadFunc,this),name),
	mutex_(),
	cond_(mutex_),
	cb_(cb){}

EventLoopThread::~EventLoopThread(){
	if (loop_ != NULL) {
		loop_->quitLoop();
		thread_.join();
	}
}

EventLoop* EventLoopThread::start(){
	thread_.start();
	EventLoop* loop = NULL;
	{
		MutexLockGuard locker(mutex_);
		while(loop_==NULL){
			cond_.wait();
		}
		loop = loop_;
	}
	return loop;
	
}

void EventLoopThread::threadFunc(){
	EventLoop loop;//避免同时有两个指针指向同一个loop
	{
		MutexLockGuard locker(mutex_);
		loop_ = &loop;
		cond_.notifyAll();
	}
	if(cb_){
		cb_();
	}
	loop.loop();//阻塞运行
	MutexLockGuard locker(mutex_);//保证在主线程loop = loop_之后
	loop_ = NULL;
	//loop是一个只存在于子线程这个函数内的临时对象
}

}

}