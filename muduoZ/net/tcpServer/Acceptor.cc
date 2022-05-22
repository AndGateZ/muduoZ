#include <fcntl.h> 
#include <string.h>
#include <errno.h>

#include "muduoZ/base/Logger.h"
#include "muduoZ/net/tcpServer/Acceptor.h"

namespace muduoZ{

namespace net{

Acceptor::Acceptor(EventLoop* loop,int port)
	:loop_(loop),
	acceptorSocket_(Socket::createNonblockSocket()),
	acceptChannel_(loop,acceptorSocket_.getFd()),
	idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
	// if(acceptorSocket_.setNoDelay()==false){
	// 	LOG<<"Acceptor setReuseAddr error";//禁用Nagle算法
	// }
	if(acceptorSocket_.setReuseAddr()==false){
		LOG<<"Acceptor setReuseAddr error";//地址重用，可以使用还处于TIME_WAIT状态的端口
	}
	//acceptorSocket_.setReusePort();//端口重用，服务器可以同时建立多个用于监听的socket，每个socket绑定的地址端口都相同
	if(acceptorSocket_.bind(port)==false){
		LOG<<"Acceptor bind port error";
	}
	acceptChannel_.setCallBackRead(std::bind(&Acceptor::handleRead,this));
}

Acceptor::~Acceptor(){
	acceptChannel_.disableAll();
	::close(idleFd_);
}


int Acceptor::listen(){
	loop_->assertInLoopThread();
	if(acceptorSocket_.listen()==false) {
		LOG<<"Acceptor listen error";
		return -1;
	}
	acceptChannel_.enableReadEvent();
	return 0;
}

void Acceptor::handleRead(){
	sockaddr addr;
	int conFd = 0;

	while(true){
		if(acceptorSocket_.accept(conFd,addr)>0){
			if(newConCallBackFunc_) newConCallBackFunc_(conFd,addr);//回调创建新的连接
			else ::close(conFd);
		}
		else {
			//是队列中没有新fd，收取完毕，退出
			if(errno==EAGAIN){
				break;
			}
			//判断是否文件描述符耗尽
			else if (errno == EMFILE){
				::close(idleFd_);//关闭描述符
				idleFd_ = ::accept(acceptorSocket_.getFd(), NULL, NULL); //接收连接请求
				::close(idleFd_); //关闭空闲文件描述符，表示已收取
				idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC); //重新打开空闲描述符
			}
			else{
				LOG<<"Acceptor errno";
				break;
			}
			
		}
	}
}

}

}