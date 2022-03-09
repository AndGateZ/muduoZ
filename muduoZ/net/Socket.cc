#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h> //read write

#include "muduoZ/net/Socket.h"

namespace muduoZ{

namespace net{

int Socket::init(){
	int fd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);//ipv4,流传输非阻塞，TCP协议
	return fd;
}

bool Socket::bind(int sockfd,int port){
	if (port < 0 || port > 65535) return false;

	struct sockaddr_in sock_addr;
	bzero((char *)&sock_addr, sizeof(sock_addr));
	sock_addr.sin_family  = AF_INET;//地址族
	//sock_addr.sin_addr.s_addr  = inet_addr("127.0.0.1");
	sock_addr.sin_addr.s_addr  = htonl(INADDR_ANY);//绑定在任意ip地址 htonl:将主机的无符号长整形数转换成网络字节顺序,小端装换成大端，因为网络地址是大端，而正常的网络地址是小端
	sock_addr.sin_port  = htons((unsigned short)port);//端口号 htons将一个无符号短整型数值转换为网络字节序，即大端模式(big-endian)

	if(::bind(sockfd, (struct sockaddr *)&sock_addr, sizeof(sockaddr)) == -1) {
		close(sockfd);
		return false;
	}
	return true;
}

bool Socket::connect(int sockfd,int port,const char* ip){
	if (port < 0 || port > 65535) return false;

	struct sockaddr_in sock_addr;
	bzero((char *)&sock_addr, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET; //地址族
	sock_addr.sin_addr.s_addr  = inet_addr(ip);
	sock_addr.sin_port  = htons((unsigned short)port); //端口号

	if(::connect(sockfd,(struct sockaddr *)&sock_addr,sizeof(sockaddr)) == -1){
		close(sockfd);
		return false;
	}
	return true;
}

bool Socket::listen(int sockfd){
	if(::listen(sockfd,128)==-1){
		close(sockfd);
		return false;
	}
	return true;
}

int Socket::accept(int sockfd){
	struct sockaddr addr;
	socklen_t addrlen = static_cast<socklen_t>(sizeof(addr));
  	bzero((char *)&addr, sizeof(addr));
	int connfd = ::accept(sockfd,&addr,&addrlen);
	return connfd;
}

ssize_t Socket::read(int sockfd, void *buf, size_t count){
	return ::read(sockfd, buf, count);
}

ssize_t Socket::write(int sockfd, void *buf, size_t count){
	return ::write(sockfd, buf, count);
}

bool Socket::close(int sockfd){
	if(::close(sockfd)==-1) return false;
	return true;
}

bool shutdownWrite(int sockfd){
	if (::shutdown(sockfd, SHUT_WR) < 0) return false;
	return true;
}

}

}