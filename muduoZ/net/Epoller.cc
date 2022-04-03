#include<string.h>
#include "muduoZ/base/Logger.h"
#include "muduoZ/net/Channel.h"
#include "muduoZ/net/Epoller.h"

namespace muduoZ{

namespace net{

const int Epoller::InitEventsSize = 16;

Epoller::Epoller(EventLoop* loop)
	:eventLoop_(loop),
	epollFd_(::epoll_create(EPOLL_CLOEXEC)),
	events_(InitEventsSize){}

Epoller::~Epoller(){
	::close(epollFd_);
}

TimeStamp Epoller::poll(int timeout,Channels& activeChannels){
	int nReady = ::epoll_wait(epollFd_,&*events_.begin(),events_.size(),timeout);
	for(int i = 0;i<nReady;++i){
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
		channel->setActiveEvent(events_[i].events);
		activeChannels.push_back(channel);
	}
	return TimeStamp::now();
}

void Epoller::updateChannel(Channel* channel){
	struct epoll_event event;
	memset(&event, 0,sizeof(event));
	event.events = channel->getEvent();
	event.data.ptr = channel;//把channel指针放入event，pollwait拿出来后填充到activeChannels

	int fd = channel->getFd();
	int state = channel->getState();
	if(state==Channel::CNew){
		::epoll_ctl(epollFd_,EPOLL_CTL_ADD,fd,&event);
		channel->setState(Channel::CAdded);
	}
	else if(state==Channel::CAdded){
		if(channel->isNoEvent()){
			::epoll_ctl(epollFd_,EPOLL_CTL_DEL,fd,&event);
			channel->setState(Channel::CDeleted);
		}
		else ::epoll_ctl(epollFd_,EPOLL_CTL_MOD,fd,&event);
	}
	else{//CDeleted
		if(!channel->isNoEvent()){
			::epoll_ctl(epollFd_,EPOLL_CTL_ADD,fd,&event);
			channel->setState(Channel::CAdded);
		}
	}
}

}

}