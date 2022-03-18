#ifndef _MUDUOZ_NET_TIMERWHEEL_
#define _MUDUOZ_NET_TIMERWHEEL_

#include <functional>
#include <vector>
#include <list>
#include <unordered_set>
#include <memory>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/TimeStamp.h"
#include "muduoZ/base/Logger.h"
#include "muduoZ/base/Mutex.h"
#include "muduoZ/net/timer/Timer.h"

namespace muduoZ{

namespace net{

class TimerWheels;
class EventLoop;

class TimerWheel:uncopyable{
public:
	typedef std::function<void()> TimerReachFunction;
	typedef std::shared_ptr<Timer> TimerPtr;
	typedef std::vector<std::list<TimerPtr>> SlotList;

	TimerWheel(int level,size_t slotNum,TimerWheels* timerWheels,int ticknOneSlot)
		:slotCur_(0),
		level_(level),
		slotNum_(slotNum),
		slotVec_(slotNum),
		timerWheels_(timerWheels),
		TickNOneSlot(ticknOneSlot){}

	~TimerWheel(){
		delAll();
	}

	void addTimer(TimerPtr timer);

	void delTimer(TimerPtr timer);
	void delAll();

	void tick();//tick+1
	void checkCurSlot();//验证本轮当前槽的timer，向下传送
	void runSlot();//最底层的轮对当前槽的timer取出

	int getSlotCur(){return slotCur_;}
	
	const size_t TickNOneSlot;//前进一位需要的tick数量
private:
	void addTimerToLower(TimerPtr timer);//用于上层给下层传送timer
	
	TimerWheels* timerWheels_;
	size_t slotNum_;//一个轮60槽，一次tick 1ms
	SlotList slotVec_;
	int slotCur_;//轮指针的位置
	int level_;//层数 最低0层

};


class TimerWheels:uncopyable{
public:
	typedef std::function<void()> TimerReachFunction;
	typedef std::shared_ptr<Timer> TimerPtr;
	typedef std::shared_ptr<TimerWheel> TimerWheelPtr;
	typedef std::vector<std::shared_ptr<TimerWheel>> WheelVec;
	typedef std::unordered_set<TimerPtr> TimerSet;

	TimerWheels(int maxLevel=5);
	~TimerWheels(){
		delAll();
	}

	//生成timer，返回timerId,扔进eventloop的queue,设定的最大定时为5天
	TimerPtr addTimer(TimeStamp expirationTime,TimerReachFunction func,size_t interval);

	void delTimer(TimerPtr timer);
	void delAll();

	TimerWheelPtr getWheel(int level) {return wheelVec_[level];}

	void tick();

	void run();
	void stop(){quit_ = true;}

	static const size_t SlotNum = 60;//一个轮60槽，一次tick 1s
	static const size_t TimerMax = 12960000;//150天

private:
	
	static const int levelMax_ = 4;//默认最大层数，4个时间轮

	WheelVec wheelVec_;//时间轮数组
	bool quit_;
	Mutex mutex_;
	//std::atomic<int> count_;

	
};


}

}

#endif
