#ifndef _MUDUOZ_NET_EPOLLER_
#define _MUDUOZ_NET_EPOLLER_

//#include <sys/poll.h>//POLLIN 
#include <sys/epoll.h>
#include <vector>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/TimeStamp.h"


namespace muduoZ{

namespace net{

class EventLoop;
class Channel;

class Epoller:uncopyable{
public:
	typedef std::vector<Channel*> Channels;

	explicit Epoller(EventLoop* loop);
	~Epoller();

	TimeStamp poll(int timeout,Channels& activeChannels);//返回时间戳,timeout阻塞时间，由loop传入
	void updateChannel(Channel* channel);//给loop的接口，更新底层epollfd

	//void assertInLoopThread();//有什么用？

	static const int InitEventsSize;//events_的初始大小
private:
	EventLoop* eventLoop_;
	int epollFd_;
	std::vector<struct epoll_event> events_;//临时变量,事件数组，其尺寸是最大监听事件数

};


}

}

#endif