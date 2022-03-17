#include <sys/timerfd.h>

#include "muduoZ/net/timer/TimerQueue.h"
#include "muduoZ/base/Logger.h"

namespace muduoZ{

namespace net{

struct timespec howMuchTimeFromNow(TimeStamp when)//计算时间间隔
{
  int64_t microseconds = when.getMicroSecondsSinceEpoch()
                         - TimeStamp::now().getMicroSecondsSinceEpoch();
  if (microseconds < 100)
  {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / TimeStamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % TimeStamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

int createTimerFd(){
	int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if(fd<0) {
		LOG<<"createTimerFd failed!";
	}
	return fd;
}

void resetTimerFd(int fd,TimeStamp expiration){
	itimerspec newVal;
	itimerspec oldVal;
	newVal.it_value = howMuchTimeFromNow(expiration);
	if(::timerfd_settime(fd,0,&newVal,&oldVal)<0){
		LOG<<"setTimerFd failed!";
	}
}

void readTimerFd(int fd){
	uint64_t num;
	if(::read(fd,&num,sizeof(num))!=sizeof(num)){
		LOG<<"readTimerFd error!";
	}
}

TimerQueue::TimerQueue(EventLoop* loop)
	:loop_(loop),
	timerFd_(createTimerFd()),
	timerChannel_(loop_,timerFd_){
		timerChannel_.enableReadEvent();
	}

TimerQueue::~TimerQueue(){
	timerChannel_.disableAll();
	::close(timerFd_);
}


TimerId TimerQueue::addTimer(TimeStamp time,double interval,TimerReachFunction func){
	if(!loop_->isInLoopThread()){
		LOG<<"addTimer error";
	}
	std::shared_ptr<Timer> timer(new Timer(TimeStamp::now().addTime(interval),func,interval));
	loop_->runInLoop(std::bind(&addTimerInLoop,this,timer));
	//扔进线程任务
	//return timerid
}

void TimerQueue::addTimerInLoop(std::shared_ptr<Timer> timer){
	if(!loop_->isInLoopThread()){
		LOG<<"addTimerInLoop error";
	}
	bool first = insertTimer(timer);
	if(first) resetTimerFd(timerFd_,timer->getExpirationTime());
}

bool TimerQueue::insertTimer(std::shared_ptr<Timer> timer){
	if(!loop_->isInLoopThread()){
		LOG<<"insertTimer error";
	}
	bool first = false;
	TimeStamp reachTime = timer->getExpirationTime();
	if(timers_.size()==0 || reachTime < timers_.begin()->first)  first = true;
	timers_.insert(std::make_pair(reachTime,timer));
	activeTimers_.insert(timer);
	return first;
}

void TimerQueue::cancelTimer(std::shared_ptr<Timer> timer){
	if(!loop_->isInLoopThread()){
		LOG<<"cancelTimer error";
	}
	//扔进线程任务
	cancelTimerInLoop(timer);
}

void TimerQueue::cancelTimerInLoop(std::shared_ptr<Timer> timer){
	if(!loop_->isInLoopThread()){
		LOG<<"cancelTimerInLoop error";
	}
	ActiveTimerSet::iterator it = activeTimers_.find(timer);
	if(it != activeTimers_.end()) {
		//无法只用timeStamp去标识唯一的timer，因此必须有timerId类作为辅助


	}

}




}

}