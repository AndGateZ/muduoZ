#ifndef _MUDUOZ_NET_EVENTLOOPTHREAD_POOL_
#define _MUDUOZ_NET_EVENTLOOPTHREAD_POOL_

#include <functional>
#include <string>
#include <vector>
#include <memory>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/Thread.h"
#include "muduoZ/net/EventLoop.h"
#include "muduoZ/net/EventLoopThread.h"

namespace muduoZ{

namespace net{

class EventLoopThreadPool:uncopyable{
public:
	typedef std::function<void()> ThreadInitCallBack;

	EventLoopThreadPool(EventLoop* baseLoop,const std::string name);
	~EventLoopThreadPool(){}

	void setThreadNum(size_t num){threadNum_ = num;}
	void start(const ThreadInitCallBack &cb);
	EventLoop* getLoop();

private:
	EventLoop* baseLoop_;//主线程有的一个loop，不实际工作，仅有单线程的情况下，任务全他做
	std::string name_;
	bool started_;
	size_t threadNum_;
	size_t nextLoopIndex_;//轮流分配任务
	std::vector<std::unique_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;
	//这里用指针，因为threads_数组成员会被析构，调用EventLoopThread的析构函数，其loop_循环将会在子线程终止，退出线程时临时对象被析构
	//所以这里无需用智能指针维护生命周期

};

}

}

#endif