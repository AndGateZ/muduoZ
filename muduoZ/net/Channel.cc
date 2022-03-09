
#include "muduoZ/net/EventLoop.h"
#include "muduoZ/net/Channel.h"

namespace muduoZ{

namespace net{

class EventLoop;

Channel::Channel(EventLoop* loop,int fd)
	:eventLoop_(loop),
	fd_(fd),
	event_(0),
	activeEvent_(0),
	tied_(false),
	state_(CAdded){}

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