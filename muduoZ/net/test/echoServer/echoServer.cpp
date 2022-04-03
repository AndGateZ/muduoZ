#include <iostream>
#include <functional>

#include "muduoZ/net/EventLoop.h"
#include "muduoZ/net/tcpServer/TcpServer.h"

using namespace muduoZ::net;
using namespace muduoZ;
using namespace std;
using namespace placeholders;

class EchoServer{
public:
	EchoServer(EventLoop* loop,int port,int threadNum):tcpServer_(loop,port,threadNum){
		tcpServer_.setConnectEstablishedCallBack(std::bind(&EchoServer::onConnectEstablished,this,_1));
		tcpServer_.setMessageReceivedCallBack(std::bind(&EchoServer::onMessageReceived,this,_1,_2,_3));
	}

	void start(){
		tcpServer_.start();
	}

private:
	void onConnectEstablished (const std::shared_ptr<Connection>& conn) {
		//cout<<"ConnectEstablished"<<endl;
	}
	void onMessageReceived(const std::shared_ptr<Connection>& conn,Buffer* buffer, TimeStamp timeStamp) {
		cout<<buffer->peek()<<endl;
		buffer->takeAll();
	}

	TcpServer tcpServer_;
};


int main(){
	EventLoop loop;
	EchoServer echoServer(&loop,8000,1);
	echoServer.start();
	loop.loop();
	return 0;
}