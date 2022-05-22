#include <functional>
#include <errno.h>
#include <memory>

#include "muduoZ/base/Logger.h"
#include "muduoZ/net/tcpServer/Connection.h"

using namespace std::placeholders;

namespace muduoZ{

namespace net{

Connection::Connection(EventLoop* loop,int sockFd,std::string name)
	:loop_(loop),
	connSock_(sockFd),
	channel_(loop_,sockFd),
	state_(Connecting),
	name_(name),
	inputBuffer_(),
	outputBuffer_()
{
	channel_.setCallBackRead(std::bind(&Connection::handleRead,this,_1));
	channel_.setCallBackWrite(std::bind(&Connection::handleWrite,this));
	channel_.setCallBackClose(std::bind(&Connection::handleClose,this));
	channel_.setCallBackError(std::bind(&Connection::handleError,this));
	connSock_.setKeepAlive();
	// connSock_.setNoDelay();//禁用Nagle算法
}

void Connection::send(const char* msg, int len){
	if (state_ == Connected){
		loop_->runInLoop(std::bind(&Connection::sendInLoop,this,msg,len));
	}
}

void Connection::send(const char* msg){
	send(msg,sizeof(msg));
}

void Connection::send(const Buffer* msg){
	send(msg->peek(),msg->getReadableLen());
}

void Connection::shutDown(){
	if (state_ == Connected){
		state_ = Disconnecting;//设置Disconnecting状态，没有在写入，直接关闭此端
		//如果在写入，跳过，但Disconnecting已经设置，handlewrite写完之后发现Disconnecting，调用shutDownInLoop，关闭此端
		loop_->runInLoop(std::bind(&Connection::shutDownInLoop,this));//无需sharedfromthis，析构流程不在不会在这里出现
  	}
}

void Connection::forceClose(){
	if (state_ == Connected || state_ == Disconnecting){
		loop_->runInLoop(std::bind(&Connection::forceCloseInLoop,shared_from_this()));//无需sharedfromthis，析构流程不在不会在这里出现
  	}
}

void Connection::connectEstablished(){
	loop_->assertInLoopThread();
	state_ = Connected;
	channel_.tie(shared_from_this());//绑定channel
	channel_.enableReadEvent();
	//用户传递进来的回调
	connectEstablishedCallBack_(shared_from_this());
}

//private
void Connection::sendInLoop(const char* msg, int len){
	loop_->assertInLoopThread();
	int remain = len;//send字符长度
	int writenLen = 0;
	bool errorFlag = false;

	//避免乱序，如果没有正在写的事件，直接写入
	if(!channel_.isWriteEvent() && outputBuffer_.getReadableLen()==0){
		writenLen = connSock_.write(msg,len);
		if(writenLen >= 0)  {
			remain -= writenLen;//剩余长度
			if(remain == 0 && messageSendCallBack_){
				loop_->runInLoop(std::bind(messageSendCallBack_,shared_from_this()));
			}
		}
		else{
			writenLen = 0;
			errorFlag = true;
			//判断error类型
		}
	}
	
	//有剩余，加入buffer并设置监听
	if(!errorFlag && remain > 0){
		outputBuffer_.append(msg+writenLen,remain);
		if(!channel_.isWriteEvent()) channel_.enableWriteEvent();
	}

}

void Connection::shutDownInLoop(){
	loop_->assertInLoopThread();
	//channel如果没有监听写事件，关闭此端的写
	//如果在监听写事件，跳过此步骤，在handlewrite写完之后会关闭监听写事件，直接调用shutdownInLoop，也就是closewait的过程
	if (!channel_.isWriteEvent()) connSock_.shutdownWrite();
}

void Connection::forceCloseInLoop(){
	loop_->assertInLoopThread();
	//判断一下状态，可能在loopqueue中已经Disconnected了
	if (state_ == Connected || state_ == Disconnecting){
		handleClose();
  	}
}

//handle
void Connection::handleRead(TimeStamp timeStamp){
	loop_->assertInLoopThread();
	// buffer+临时栈反复收取信息直到收完
	int savedErrno = 0;
	int receiveSum = inputBuffer_.readFd(connSock_.getFd(),&savedErrno);
	// 读到>0 调用回调
	if(receiveSum > 0 && messageReceivedCallBack_){
		messageReceivedCallBack_(shared_from_this(),&inputBuffer_,timeStamp);
	}
	// 读到0 调用handleclose
	else if(receiveSum == 0){
		handleClose();
	}
	// 读到<0 调用handleError
	else{
		errno = savedErrno;
		handleError();
	}

}
void Connection::handleWrite(){
	loop_->assertInLoopThread();
	//1 buffer发送
	int savedErrno = 0;
	int sendSum = outputBuffer_.writeFd(connSock_.getFd(),&savedErrno);
	// 写>0 
	if(sendSum > 0){
		outputBuffer_.haveReadForLen(sendSum);
		//全部写完
		if(outputBuffer_.getReadableLen() == 0){
			channel_.disableWriteEvent();
			if(messageSendCallBack_) {
				loop_->queueInLoop(std::bind(messageSendCallBack_, shared_from_this()));
			}
			//关闭连接的状态，close_wait
			if(state_==Disconnecting) {
				shutDownInLoop();
			}
		}
	}
	// 写<=0 调用handleError
	else{
		errno = savedErrno;
		LOG<<"Connection::handleWrite error";
	}

}

void Connection::handleError(){
	//error
}

void Connection::handleClose(){
	state_ = Disconnected;
	channel_.disableAll();
	std::shared_ptr<Connection> connPtr(shared_from_this());

	//?
	connectCloseCallBack_(connPtr);//TCP的connectionClose,析构就close了
}


}

}