#ifndef _MUDUOZ_NET_TIMER_
#define _MUDUOZ_NET_TIMER_

#include <functional>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/TimeStamp.h"

namespace muduoZ{

namespace net{

class Timer:uncopyable{
public:
	typedef std::function<void()> TimerReachFunction;

	Timer(TimeStamp expirationTime,TimerReachFunction &timerReachFunction,double interval = 0.0)
		:expirationTime_(expirationTime),
		interval_(interval),
		repeat_(interval>0.0),
		func_(timerReachFunction){}

	~Timer();

	bool isRepeated() {return repeat_;}
	TimeStamp getExpirationTime() {return expirationTime_;}
	void callFunc() { func_(); }
	void resetTimer(){
		if(repeat_) expirationTime_.addTime(interval_);
		else  expirationTime_ = TimeStamp::invalid();
	}

private:
	TimeStamp expirationTime_;
	const bool repeat_;
	const double interval_;
	TimerReachFunction func_;
	
};

}

}

#endif