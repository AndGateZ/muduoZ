
#include <sys/eventfd.h>

#include "muduoZ/base/Logger.h"
#include "muduoZ/base/CurrentThread.cc"
#include "muduoZ/net/EventLoop.h"
#include "muduoZ/net/Epoller.h"
#include "muduoZ/net/Socket.h"
#include "muduoZ/net/Channel.h"

namespace muduoZ{

namespace net{

typedef std::shared_ptr<Timer> TimerPtr;

__thread EventLoop* t_loopInThisThread = NULL;

int createEventFd(){
	int eventFd = ::eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
	return eventFd;
}
EventLoop* EventLoop::getEventLoopOfCurrentThread() const {
	return t_loopInThisThread;
}

EventLoop::EventLoop()
	:epoller_(new Epoller(this)),
	wakeupFd_(createEventFd()),
	wakeupChannel_(new Channel(this,wakeupFd_)),
	callingPendingFunctors_(false),
	quit_(false),
	looping_(false),
	threadId_(CurrentThread::tid())
	{
		if (t_loopInThisThread){
			//muduoZ::LOG<<
		}
		else{
			t_loopInThisThread = this;
		}
		wakeupChannel_->setCallBackRead(std::bind(&EventLoop::handleRead,this));//这里无需绑定参数，因为调用的时候有参数
		wakeupChannel_->enableReadEvent();
		//muduoZ::LOG<<"EventPoll start";
	}

EventLoop::~EventLoop(){
	wakeupChannel_->disableAll();
	::close(wakeupFd_);
	t_loopInThisThread = NULL;
}

void EventLoop::loop(){
	looping_ = true;
	quit_ = false;
	while(!quit_){
		activeChannels_.clear();
		TimeStamp now = epoller_->poll(kPollTimeMs,activeChannels_);//kPollTimeMs阻塞，1s必定唤醒一次，与事件无关
		for(Channel* channel:activeChannels_){
			currentActiveChannel_ = channel;
			currentActiveChannel_->handleEvent(now);
		}
		currentActiveChannel_ = NULL;
		doPendingFunctions();
	}
	looping_ = false;
}

void EventLoop::quitLoop(){
	quit_ = true;
	if (!isInLoopThread()){
		wakeup();
	}
}

void EventLoop::runInLoop(Function function){
	if(isInLoopThread()){
		function();
	}
	else{
		queueInLoop(std::move(function));
	}
}

void EventLoop::queueInLoop(Function function){
	{
		MutexLockGuard locker(mutex_);//这里需要锁，禁止多线程同时在自己的eventloop中加入任务
		pendingFunctions_.push_back(std::move(function));
	}
	if(!isInLoopThread()||callingPendingFunctors_){
		wakeup();
	}
	//如果是其他线程调用自己的runInLoop，直接唤醒
  	//如果callingPendingFunctors_==true，意味着这个queueInLoop是前面pendingFunctors_函数调用的，为了保证这次调用的连续性，提前把epoll唤醒了
  	//让前面pendingFunctors_里的某个函数新加入pendingFunctors_的函数能够连续执行完毕

}

void EventLoop::assertInLoopThread(){
	if(!isInLoopThread()) {
		LOG<<"not in Loop Thread";
		fflush(stdout);
		abort();
	}
}

bool EventLoop::isInLoopThread(){
	return threadId_ == CurrentThread::tid();
}

void EventLoop::doPendingFunctions(){
	std::vector<Function> functions;
	callingPendingFunctors_ = true;
	{
		MutexLockGuard locker(mutex_);
		functions.swap(pendingFunctions_);
	}
	for(const Function& f:functions) f();
}

void EventLoop::wakeup(){
	uint64_t t = 0;
	ssize_t n = ::write(wakeupFd_,&t,sizeof(t));
	//muduoZ::LOG<<"wakeup write complete";
	if(n<sizeof(t)) {
		//muduoZ::LOG<<"wakeup failed";
	}
}

void EventLoop::updateChannel(Channel* channel){
	epoller_->updateChannel(channel);
}

void EventLoop::handleRead(){
	uint64_t t = 0;
	ssize_t n = ::read(wakeupFd_,&t,sizeof(t));
	if(n<sizeof(t)) {
		//log
	}
}

}

}