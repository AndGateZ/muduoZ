#include <string>

#include "muduoZ/base/Logger.h"
#include "muduoZ/net/tcpServer/TcpServer.h"

using namespace std::placeholders;


namespace muduoZ{

namespace net{

TcpServer::TcpServer(EventLoop* baseLoop,int port,int threadNum,const std::string name,bool timerWheel)
	:baseLoop_(baseLoop),
	acceptor_(baseLoop_,port),
	threadPool_(baseLoop_,threadNum,"ThreadPool",timerWheel),
	connectionId_(0),
	port_(std::to_string(port)),
	name_(name)
{
	acceptor_.setNewConnectionCallBack(std::bind(&TcpServer::newConnection,this,_1,_2));
	
}


void TcpServer::start(){
	if(threadInitCallBack_) threadPool_.start(threadInitCallBack_);
	else threadPool_.start(std::bind(&TcpServer::defaultThreadInitCallBack,this));
	//TCPServer baseloop运行于主线程
	baseLoop_->runInLoop(std::bind(&Acceptor::listen, &acceptor_));
}

//由acceptor调用
void TcpServer::newConnection(int sockfd, const sockaddr &addr){
	baseLoop_->assertInLoopThread();

	EventLoop* loop = threadPool_.getLoop();
	//connectionMap
	char buf[64];
	snprintf(buf,sizeof(buf),"%s-%s-%d",name_.c_str(),port_.c_str(),connectionId_);
	++connectionId_;
	std::string name = buf;
	//new Connection
	ConnectionPtr conn(new Connection(loop,sockfd,name));
	connectionMap_[name] = conn;
	conn->setConnectEstablishedCallBack(connectEstablishedCallBack_);
	conn->setConnectCloseCallBack(std::bind(&TcpServer::connectionClose,this,_1));
	conn->setMessageReceivedCallBack(messageReceivedCallBack_);
	conn->setMessageSendCallBack(messageSendCallBack_);
	//run established in loop
	loop->queueInLoop(std::bind(&Connection::connectEstablished,conn));
}

//connection调用，为了在map中清除connection生命的最后一个计数，需要运行在tcpServer线程
void TcpServer::connectionClose(const ConnectionPtr &connection){
	baseLoop_->runInLoop(std::bind(&TcpServer::connectionCloseInLoop, this, connection));
}

void TcpServer::connectionCloseInLoop(const ConnectionPtr &connection){
	baseLoop_->assertInLoopThread();
	connectionMap_.erase(connection->getName());
	//EventLoop* loop = connection->getLoop();
	//最后执行connection的销毁回调
	//loop->queueInLoop(std::bind(&Connection::connectDestroyed, connection));
}

void TcpServer::defaultThreadInitCallBack(){}

}

}