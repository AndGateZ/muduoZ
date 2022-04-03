
#include "muduoZ/net/EventLoop.h"
#include "muduoZ/net/tcpServer/TcpServer.h"
#include "muduoZ/net/HttpServer/HttpRequest.h"
#include "muduoZ/net/HttpServer/HttpResponse.h"
#include "muduoZ/base/Logger.h"

using namespace std::placeholders;

namespace muduoZ{

namespace net{

class HttpServer{
public:
	HttpServer(EventLoop* loop,int port,int threadNum):tcpServer_(loop,port,threadNum){
		tcpServer_.setConnectEstablishedCallBack(std::bind(&HttpServer::onConnection,this,_1));
		tcpServer_.setMessageReceivedCallBack(std::bind(&HttpServer::onMessage,this,_1,_2,_3));
	}

	void start(){
		tcpServer_.start();
	}

private:
	void onConnection(const std::shared_ptr<Connection>& conn){
		if (conn->connected()){
			//LOG<<"onConnection "<<conn->getName().c_str();
			conn->setContext(HttpRequest());
		}
	}

	void onMessage(const std::shared_ptr<Connection>& conn,Buffer* buffer, TimeStamp timeStamp){
		HttpRequest* req = boost::any_cast<HttpRequest>(conn->getContext());
		if(!req->parseMessage(buffer,timeStamp)){
			LOG<<"receive Bad Request";
			conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
			conn->shutDown();
		}

		//LOG<<conn->getName().c_str()<<" "<<req<<" "<<req->getState();
		//LOG<<tcpServer_.getMapSize();
		
		if(req->getState()==HttpRequest::GotAll){
			//get info
			const string& connection = req->getHeader("Connection");
			bool keepConnection = true;
			if(connection == "close" || req->getVersion()==HttpRequest::Http10 && connection != "Keep-Alive" ){
				keepConnection = false;
			}
			//make response
			HttpResponse response(keepConnection);
			response.setCode(HttpResponse::C200Ok);
			response.setCodeMsg("OK");
			//body
			Buffer msg;
			response.addToBuffer(&msg);
			//LOG<<msg.peek();
			conn->send(&msg);
			//shutdown
			if (keepConnection==false)	conn->shutDown();
			req->reset();
		}

	}

	TcpServer tcpServer_;

};

}

}

using namespace muduoZ::net;

int main(){
	EventLoop loop;
	HttpServer httpServer(&loop,8000,4);
	httpServer.start();
	loop.loop();
	return 0;
}
