
#include "muduoZ/net/timer/TimerWheelsThread.h"

namespace muduoZ{

namespace net{

TimerWheelsThread::TimerWheelsThread()
	:timerWheels_(NULL),
	mutex_(),
	cond_(mutex_),
	thread_(std::bind(&TimerWheelsThread::threadFunc,this),"TimerWheel")
	{}

TimerWheelsThread::~TimerWheelsThread(){
	if(timerWheels_!=NULL){
		timerWheels_->stop();
		thread_.join();
	}
	
}

TimerWheels* TimerWheelsThread::start(){
	TimerWheels* timerWheels = NULL;
	thread_.start();
	{
		MutexLockGuard locker(mutex_);
		while(timerWheels_==NULL) cond_.wait();
		timerWheels = timerWheels_;
	}
	return timerWheels;
}

void TimerWheelsThread::threadFunc(){
	TimerWheels timerWheels;
	{
		MutexLockGuard locker(mutex_);
		timerWheels_ = &timerWheels;
		cond_.notifyAll();
	}
	timerWheels.run();
	MutexLockGuard locker(mutex_);
	timerWheels_ = NULL;
}

}

}