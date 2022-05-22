#ifndef _MUDUOZ_NET_SOCKET_
#define _MUDUOZ_NET_SOCKET_
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h> 

#include "muduoZ/base/uncopyable.h"

namespace muduoZ{

namespace net{

class Socket:uncopyable{
public:
	//explicit Socket(int sockfd):fd_(sockfd){ }
	explicit Socket(int sockfd):fd_(sockfd){ fcntl(sockfd, F_SETFL, O_NONBLOCK);  }
	~Socket(){::close(fd_);}

	int getFd() const {return fd_;};
	
	static int createNonblockSocket();
	bool bind(int port);//成功返回true,绑定在任何ip
	bool connect(int port,const char* ip);
	bool listen();
	int accept();//返回fd，失败返回-1
	int accept(int &conFd,sockaddr &addr); //传入addr，传出fd
	ssize_t read(void *buf, size_t count);//成功返回读出的字节数，出错返回-1并设置errno
	ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
	ssize_t write(const char *buf, size_t count);//成功返回写入的字节数，出错返回-1并设置errno
	bool close();
	bool shutdownWrite();//关闭sockfd的写功能，此选项将不允许sockfd进行写操作，即进程不能在对此套接字发出写操作。

	bool setReuseAddr();//地址重用,可以使用还处于TIME_WAIT状态的端口
	bool setReusePort();//端口重用,服务器可以同时建立多个用于监听的socket，每个socket绑定的地址端口都相同，内核会采用负载均衡的方法将每个将每个客户端请求分配给某一个socket，可以很大程序的提高并发性，充分利用CPU资源
	bool setKeepAlive();//
	bool setNoDelay();

private:
	const int fd_;

};

}

}

#endif