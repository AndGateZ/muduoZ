#ifndef _MUDUOZ_NET_CONNECTION_
#define _MUDUOZ_NET_CONNECTION_

#include <functional>
#include <string>

#include "muduoZ/base/uncopyable.h"
#include "muduoZ/base/TimeStamp.h"
#include "muduoZ/net/Socket.h"
#include "muduoZ/net/Channel.h"
#include "muduoZ/net/EventLoop.h"
#include "muduoZ/net/tcpServer/Buffer.h"
#include "boost/any.hpp"

namespace muduoZ{

namespace net{

class Connection : uncopyable, public std::enable_shared_from_this<Connection>
{
public:
	typedef std::function< void(const std::shared_ptr<Connection>&) > ConnectEstablishedCallBack;
	typedef std::function< void(const std::shared_ptr<Connection>&) > MessageSendCallBack;
	typedef std::function< void(const std::shared_ptr<Connection>&, Buffer*, TimeStamp) > MessageReceivedCallBack;
	typedef std::function< void(const std::shared_ptr<Connection>&) > ConnectCloseCallBack;
	//1 上层tcpserver调用bind扔进queueinloop的时候，绑定类的函数需要对象指针
	//2 使用const shared_ptr& 可以减少一次计数，直接调用指针而不是作为参数复制一个 
	//3 参数均用占位符，在connnection调用的时候填充参数 

	explicit Connection(EventLoop* loop,int sockFd,std::string name);
	//~Connection();

	void send(const char* msg, int len);
	void send(const char* msg);
	void send(const Buffer* msg);
	void shutDown();
	void forceClose();

	//暂时不知道什么用
	//void startRead();//开始读监听
	//void stopRead();//停止读监听

	void connectEstablished();//acceptor-TcpServer调用 ,开启channel监听，设置状态，执行用户回调
	//void connectDestroyed(); //handleClose-TcpServer调用,关闭channel监听，移除epollfd，设置状态,退出后此conn将被析构

	//get
	EventLoop* getLoop() const {return loop_;}
	const std::string& getName() const {return name_;}

	//回调设置
	void setConnectEstablishedCallBack(const ConnectEstablishedCallBack &cb){connectEstablishedCallBack_=cb;}
	void setMessageReceivedCallBack(const MessageReceivedCallBack &cb){messageReceivedCallBack_=cb;}
	void setMessageSendCallBack(const MessageSendCallBack&cb){messageSendCallBack_=cb;}
	void setConnectCloseCallBack(const ConnectCloseCallBack &cb){connectCloseCallBack_=cb;}

	//业务相关
	void setContext(const boost::any& context){ context_ = context; }
	boost::any* getContext() { return &context_; }

	//conn
	bool connected() const {return state_==Connected;}

private:
	//InLoop均为要调用所属的线程来操作loop，其他线程操作loop会出现线程间竞争，保证当前thread操作loop是线程安全的
	void sendInLoop(const char* msg, int len);//主动写入buffer，如果有剩余就加入buffer，设置channel写监听
	void shutDownInLoop();//关闭写
	void forceCloseInLoop();//直接手动调用handleClose
	//void startReadInloop();
	//void stopReadInLoop();

	void handleRead(TimeStamp timeStamp);//socket触发读操作，利用局部变量反复写入buffer读到空为止，执行messageSendCallBack_回调
	void handleWrite();//socket触发写操作，如果写完了buffer就取消写监听
	void handleError();
	void handleClose();

	enum State{ Connecting, Connected, Disconnecting, Disconnected };
	State state_;
	EventLoop* loop_;
	Socket connSock_;
	Channel channel_;
	const std::string name_;

	Buffer inputBuffer_;//读
	Buffer outputBuffer_;//写

	//上层设定的回调，在相应的handle中调用
	ConnectEstablishedCallBack connectEstablishedCallBack_;//用户
	MessageReceivedCallBack messageReceivedCallBack_;//用户
	MessageSendCallBack messageSendCallBack_;//用户
	ConnectCloseCallBack connectCloseCallBack_;//TcpServer

	//业务处理对象
	boost::any context_;

};


}

}

#endif