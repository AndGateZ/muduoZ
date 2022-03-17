#ifndef _MUDUOZ_NET_TIMERQUEUE_
#define _MUDUOZ_NET_TIMERQUEUE_

#include <functional>
#include <memory>
#include <set>
#include <vector>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/TimeStamp.h"
#include "muduoZ/net/timer/Timer.h"
#include "muduoZ/net/timer/TimerId.h"
#include "muduoZ/net/Channel.h"
#include "muduoZ/net/EventLoop.h"

namespace muduoZ{

namespace net{

class TimerQueue:uncopyable{
public:
	typedef std::set<std::pair<TimeStamp,std::shared_ptr<Timer>>> TimerSet;
	typedef std::set<std::shared_ptr<Timer>> ActiveTimerSet;
	typedef std::vector<std::shared_ptr<Timer>> TimerVec;
	typedef std::function<void()> TimerReachFunction;

	TimerQueue(EventLoop* loop);
	~TimerQueue();

	TimerId addTimer(TimeStamp time,double interval,TimerReachFunction func);
	void cancelTimer(TimerId timerId);

private:
	void addTimerInLoop(std::shared_ptr<Timer> timer);
	void cancelTimerInLoop(std::shared_ptr<Timer> timer);
	void handleRead();
	TimerVec getExpirationTimers();
	bool insertTimer(std::shared_ptr<Timer> timer);//返回是否是接下来的第一个到期,如果是第一个，需要刷新fd

	TimerSet timers_;
	ActiveTimerSet activeTimers_;//用于删除timers_中的定时器，通过用timer寻找timers的淘汰时间戳
	EventLoop* loop_;

	int timerFd_;
	Channel timerChannel_;


};

}

}

#endif