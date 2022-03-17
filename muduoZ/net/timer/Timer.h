#ifndef _MUDUOZ_NET_TIMER_
#define _MUDUOZ_NET_TIMER_

#include <functional>
#include <memory>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/copyable.h"
#include "muduoZ/base/TimeStamp.h"

namespace muduoZ{

namespace net{

class Timer:uncopyable{
public:
	typedef std::function<void()> TimerReachFunction;
	Timer(size_t tickNum, int slot, int level, TimerReachFunction timerReachFunction, size_t interval = 0)
		:tickNum_(tickNum),
		slot_(slot),
		level_(level),
		interval_(interval),
		repeat_(interval>0),
		func_(std::move(timerReachFunction))
		{}

	bool isRepeated() const {return repeat_;}
	
	void callFunc() { func_(); }
	TimerReachFunction getFunc(){return func_;}

	size_t getTickNum() const {return tickNum_;}

	void setTickNum(size_t tickNum) {tickNum_ = tickNum;}

	void setSlot(int slot) { slot_ = slot; }
	int getSlot() const { return slot_; }

	void levelDecrease() { level_--; }
	int getLevel() const { return level_; }
	void setLevel(int level) { level_ = level; }


	size_t getInterval() const { return interval_; }

private:
	size_t tickNum_;//第一次开启总共需要tick的次数
	int slot_;//记录timer位于wheel的哪一个槽中，实质是key，用哈希或者数组
	int level_;//记录timer在哪一层中
	const bool repeat_;
	const size_t interval_;//只能以ms为单位,重复定时时钟的间隔tick
	TimerReachFunction func_;
};

// class TimerId:copyable{
// public:
// 	TimerId(std::shared_ptr<Timer> timer,int64_t id)
// 	:timer_(timer),
// 	id_(id){}

// 	int64_t getId() const {return id_;}
// 	std::shared_ptr<Timer> getTimer() const {return timer_;}

// private:	
// 	std::shared_ptr<Timer> timer_;
// 	int64_t id_;
// };


}

}

#endif