#ifndef _MUDUOZ_NET_TIMERWHEELS_THREAD_
#define _MUDUOZ_NET_TIMERWHEELS_THREAD_

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/Thread.h"
#include "muduoZ/base/Mutex.h"
#include "muduoZ/base/Condition.h"
#include "muduoZ/net/timer/TimerWheel.h"

namespace muduoZ{

namespace net{

class TimerWheelsThread:uncopyable{
public:
	TimerWheelsThread();
	~TimerWheelsThread();

	TimerWheels* start();

private:
	void threadFunc();//绑定子线程的启动函数

	TimerWheels* timerWheels_;
	Mutex mutex_;
	Condition cond_;
	Thread thread_;
	

};

}

}




#endif