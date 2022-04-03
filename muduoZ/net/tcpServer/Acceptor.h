#ifndef _MUDUOZ_NET_ACCEPTOR_
#define _MUDUOZ_NET_ACCEPTOR_

#include <functional>
#include <sys/socket.h>

#include "muduoZ/net/Socket.h"
#include "muduoZ/net/Channel.h"
#include "muduoZ/net/EventLoop.h"
#include "muduoZ/base/uncopyable.h"

namespace muduoZ{

namespace net{

class Acceptor:uncopyable{
public:
	typedef std::function<void (int sockfd, const sockaddr &addr)> NewConnectionCallBack;

	Acceptor(EventLoop* loop,int port);
	~Acceptor();

	int listen();//失败返回-1

	void setNewConnectionCallBack(const NewConnectionCallBack &func){newConCallBackFunc_ = func; }

private:
	void handleRead();//channel的callback

	EventLoop* loop_;
	Socket acceptorSocket_;
	Channel acceptChannel_;
	NewConnectionCallBack newConCallBackFunc_;
	int idleFd_;//为避免fd耗尽的方案：用此fd接收连接后立刻关闭

};

}

}


#endif