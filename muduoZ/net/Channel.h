#ifndef _MUDUOZ_NET_CHANNEL_
#define _MUDUOZ_NET_CHANNEL_

#include <memory>
#include <functional>
#include <sys/poll.h>//POLLIN
#include <sys/epoll.h>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/TimeStamp.h"

namespace muduoZ{

namespace net{

class EventLoop;

class Channel:uncopyable{
public:
	typedef std::function<void()> CallBackFunction;
	typedef std::function<void(TimeStamp)> CallBackFunctionT;//读操作的回调可能需要延迟，因此有时间参数

	Channel(EventLoop* loop,int fd);//在父类实体中创建好fd，绑定loop
	~Channel();

	//get
	EventLoop* getEventLoop() const {return eventLoop_;}
	int getFd() const {return fd_;}
	

	//事件相关
	int getEvent() const {return event_;}
	void setEvent(int event){event_ = event;}//by 上层父亲实体
	void setActiveEvent(int event){activeEvent_ = event;};//by poller 监听到活动
	
	
	//监听相关
	//判断监听event类型
	bool isNoEvent() const {return event_ == NoneEvent;}
	bool isReadEvent() const {return event_ & ReadEvent;}
	bool isWriteEvent() const {return event_ & WriteEvent;}
	//设置监听event类型 by 上层父亲实体,按位与
	void disableAll() { event_ = NoneEvent; updateChannel(); }
	void enableReadEvent() { event_ |= ReadEvent; updateChannel(); }
	void enableWriteEvent() { event_ |= WriteEvent; updateChannel(); }
	void disableReadEvent() { event_ &= ~ReadEvent; updateChannel(); }
	void disableWriteEvent() { event_ &= ~WriteEvent; updateChannel(); }

	//回调相关
	void handleEvent(TimeStamp receiveTime);//loop里执行，根据事件类型执行回调，判断是否绑定，这个时间是poll返回的瞬间
	void setCallBackRead(CallBackFunctionT f) { callBackRead_ = std::move(f);}
	void setCallBackWrite(CallBackFunction f) { callBackWrite_ = std::move(f);}
	void setCallBackClose(CallBackFunction f) { callBackClose_ = std::move(f);}
	void setCallBackError(CallBackFunction f) { callBackError_ = std::move(f);}


	//状态机相关
	int getState() const { return state_; }
  	void setState(int state) { state_ = state; }
	static const int CNew;//channel的状态机
	static const int CAdded;
	static const int CDeleted;

	//tie,与上层实体绑定
	void tie(const std::shared_ptr<void>& obj);

private:
	void handleEventWithoutGuard(TimeStamp receiveTime);//真正的执行回调函数
	void updateChannel();//调用loop，调用epoller，更新底层event,设置监听的时候调用

	EventLoop* eventLoop_;
	int fd_;//负责这个文件描述符的io事件分发
	int event_;//监听的事件类型
	int activeEvent_;//正在发送的事件类型
	bool tied_;//标识是否和父类实体绑定过
	std::weak_ptr<void> tie_;//接收来自上层绑定时候的实体，为上层延长生命周期
	int state_; // 状态机 used by Poller 表示channel的状态，add deleted...

	//回调函数
	CallBackFunctionT callBackRead_;
	CallBackFunction callBackWrite_;
	CallBackFunction callBackClose_;
	CallBackFunction callBackError_;

	static const int NoneEvent;
	static const int ReadEvent;//EPOLLET为边缘触发，默认是水平触发
	static const int WriteEvent;

	
};

}


}

#endif