
#include "muduoZ/net/EventLoop.h"
#include "muduoZ/net/Channel.h"

namespace muduoZ{

namespace net{

const int Channel::CNew = -1;//channel的状态机
const int Channel::CAdded = 1;
const int Channel::CDeleted = 2;

const int Channel::NoneEvent = 0;
const int Channel::ReadEvent = EPOLLIN | EPOLLPRI | EPOLLET;//EPOLLET为边缘触发，默认是水平触发
const int Channel::WriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop,int fd)
	:eventLoop_(loop),
	fd_(fd),
	event_(0),
	activeEvent_(0),
	tied_(false),
	state_(CNew){}

Channel::~Channel(){}

void Channel::tie(const std::shared_ptr<void>& obj){
	if(!tied_){
		tie_ = obj;
		tied_ = true;
	}
}

void Channel::handleEvent(TimeStamp receiveTime){
	if(tied_){
		std::shared_ptr<void> guard;
		guard = tie_.lock();
		if(guard){
			handleEventWithoutGuard(receiveTime);
		}
	}
	else{
		handleEventWithoutGuard(receiveTime);
	}
	//handleEvent可能执行有销毁上层实体的回调
	//上层实体在建立后需要tie绑定channel，在这里guard保证上层不会被析构，至少在这条语句结束后生命计数为0
}

void Channel::handleEventWithoutGuard(TimeStamp receiveTime){
	//文件描述符被挂断 且不是可读的事件
	if ((activeEvent_ & POLLHUP) && !(activeEvent_ & POLLIN)){
		if (callBackClose_) callBackClose_();
	}

	//文件描述符非法
	if (activeEvent_ & (POLLERR | POLLNVAL)){
		//log
		if (callBackError_) callBackError_();
	}

	//读 挂起
	if (activeEvent_ & (POLLIN | POLLPRI | POLLRDHUP)){
		if (callBackRead_) callBackRead_(receiveTime);
	}

	//写事件
	if (activeEvent_ & POLLOUT)
	{
		if (callBackWrite_) callBackWrite_();
	}
}

void Channel::updateChannel(){
	eventLoop_->updateChannel(this);
}



}


}