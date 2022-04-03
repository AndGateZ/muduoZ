
#include <stdio.h>

#include "muduoZ/net/EventLoopThreadPool.h"

namespace muduoZ{

namespace net{
typedef std::shared_ptr<Timer> TimerPtr;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop,int threadNum,const std::string name,bool timerWheel)
	:baseLoop_(baseLoop),
	name_(name),
	timerWheelsEnable_(timerWheel),
	timerWheels_(NULL),
	threadNum_(threadNum),
	nextLoopIndex_(0){}

void EventLoopThreadPool::start(const ThreadInitCallBack &cb){
	if(threadNum_>0){
		for(int i = 0;i<threadNum_;++i){
			char buf[name_.size() + 32];
			snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
			EventLoopThread* t = new EventLoopThread(cb, buf);
			threads_.push_back(std::unique_ptr<EventLoopThread>(t));
			loops_.push_back(t->start());
		}
	}
	else if(cb){
		cb();
	}

	if(timerWheelsEnable_){
		TimerWheelsThread* t = new TimerWheelsThread();
		timerWheelsThread_.reset(t);
		timerWheels_ = t->start();
	}
	
}

EventLoop* EventLoopThreadPool::getLoop(){
	EventLoop* loop = baseLoop_;
	if (!loops_.empty()){
		loop = loops_[nextLoopIndex_];
		(++nextLoopIndex_)%threadNum_;
		if (nextLoopIndex_ >= loops_.size()) nextLoopIndex_ = 0;
	}
	return loop;
}


TimerPtr EventLoopThreadPool::runAt(TimeStamp time,TimerReachFunction func){
	return timerWheels_->addTimer(time,std::move(func),0);
}

TimerPtr EventLoopThreadPool::runAfter(size_t milliSecond,TimerReachFunction func){
	return timerWheels_->addTimer(TimeStamp::now().addTime(milliSecond),std::move(func),0);
}

TimerPtr EventLoopThreadPool::runEvery(size_t milliSecond,TimerReachFunction func){
	return timerWheels_->addTimer(TimeStamp::now().addTime(milliSecond),std::move(func),milliSecond);
}

void EventLoopThreadPool::cabcelTimer(TimerPtr timer){
	timerWheels_->delTimer(timer);
}

}

}
