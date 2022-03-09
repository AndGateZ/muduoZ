#ifndef _MUDUOZ_NET_SOCKET_
#define _MUDUOZ_NET_SOCKET_

#include "muduoZ/base/uncopyable.h"

namespace muduoZ{

namespace net{

class Socket:uncopyable{
public:
	explicit Socket(int sockfd):fd_(sockfd){}
	~Socket(){::close(fd_);}

	int getFd() const {return fd_;};
	
	static int init();
	static bool bind(int sockfd,int port);//成功返回true
	static bool connect(int sockfd,int port,const char* ip);
	static bool listen(int sockfd);
	static int accept(int sockfd);//返回fd，失败返回-1
	static ssize_t read(int sockfd, void *buf, size_t count);//成功返回读出的字节数，出错返回-1并设置errno
	static ssize_t write(int sockfd, void *buf, size_t count);//成功返回写入的字节数，出错返回-1并设置errno
	static bool close(int sockfd);
	static bool shutdownWrite(int sockfd);

private:
	const int fd_;

};

}

}

#endif