#ifndef _MUDUOZ_NET_EVENTLOOP_
#define _MUDUOZ_NET_EVENTLOOP_

#include <vector>
#include <memory>
#include <functional>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/TimeStamp.h"
#include "muduoZ/base/Mutex.h"
#include "muduoZ/net/timer/TimerWheel.h"
#include "muduoZ/net/timer/Timer.h"

namespace muduoZ{

namespace net{

class Epoller;
class Channel;
class Socket;

class EventLoop:uncopyable{
public:
	typedef std::function<void()> Function;
	typedef std::shared_ptr<Timer> TimerPtr;
	typedef std::function<void()> TimerReachFunction;
	EventLoop();
	~EventLoop();

	//get
	EventLoop* getEventLoopOfCurrentThread() const ;

	//核心
	void loop();
	void quitLoop();
	void runInLoop(Function function);
	void queueInLoop(Function function);
	void assertInLoopThread();
	bool isInLoopThread();
	void doPendingFunctions();

	//定时任务

	//wakeupchannel唤醒
	void wakeup();

	//epollfd:由channel调用，令epoll对底层更新
	void updateChannel(Channel* channel);

private:
	void handleRead();//wakeupchannel唤醒的回调

	//poller,唯一持有
	std::unique_ptr<Epoller> epoller_;
	static const int kPollTimeMs = 1000;

	//wakeup，唯一持有
	int wakeupFd_;
	std::unique_ptr<Channel> wakeupChannel_;

	//poll中获取channels的局部变量
	std::vector<Channel*> activeChannels_;
	Channel* currentActiveChannel_;
	TimeStamp pollReturnTime_;//poll收到数据时刻的时间戳变量

	//一些状态变量
	bool callingPendingFunctors_;//是否正在执行任务队列的函数，用于判断queueInLoop的wakeup
	bool quit_;//用于控制loop结束
	bool looping_;

	//任务队列
	std::vector<Function> pendingFunctions_;

	//锁
	Mutex mutex_;

	//线程id
	const pid_t threadId_;//本对象线程的ID，构造函数的时候初始化
	

};

}

}

#endif