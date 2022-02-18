#include <sys/socket.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <poll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define LISTENQ (1024)
#define MAXLINE (1024)
#define MAXCLIENT (1024)

//fd_set  set;
//FD_ZERO(&set);        /*将set清零使集合中不含任何fd*/
//FD_SET(fd, &set);      /*将fd加入set集合*/
//FD_CLR(fd, &set);      /*将fd从set集合中清除*/
//FD_ISSET(fd, &set);   /*测试fd是否在set集合中*/

int main(void) {
    pid_t pid = getpid();
    printf("pid: %d\n", pid);

    //初始化获得一个socket监听套接字的文件描述符，存储于/proc/<pid>/fd/文件夹下
    // { * : 9999, * : * }
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    printf("socket_fd: %d\n", socket_fd);

    //初始化socket信息数据结构
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9999);
    
    //将socket与地址：端口关联起来，发送到这个地址：端口的数据可以通过这个socket读取和使用
    bind(socket_fd, (const struct sockaddr *) &server_address, sizeof(server_address));
    //开始监听socket
    listen(socket_fd, LISTENQ);

    struct sockaddr_in client_address;
    char buffer[MAXLINE];
    ssize_t n;
    socklen_t len = sizeof(client_address);

    //下面是Epool部分
    // events 为 epoll_wait 函数输出结果所用的数组

    //typedef union epoll_data
    //{
    //void *ptr;
    //int fd;
    //uint32_t u32;
    //uint64_t u64;
    //} epoll_data_t;

    //struct epoll_event
    //{
    //uint32_t events;	/* Epoll events */
    //epoll_data_t data;	/* User data variable */
    //} __EPOLL_PACKED;

    struct epoll_event events[MAXCLIENT], ev;

    // 与 select 和 poll 不同，epoll 会创建新的文件描述符，监控和管理句柄 fd 的文件
    int epoll_fd = epoll_create(MAXCLIENT);
    printf("epoll_fd: %d\n", epoll_fd);

    // 监视 EPOLLIN 事件，也可以设置触发方式，默认水平触发
    ev.events = EPOLLIN;

    // ev.data 是自由写入的。epoll_wait 返回的时候，会将 ev.data 原样返回出来
    ev.data.fd = socket_fd;

    // 将 socket_fd 加入监视。当 socket_fd 发生 EPOLLIN 事件时，会将此时写好的 ev.data 的值返回出来
    //参数：监听文件描述符，监听事件类型，监听socket的文件描述符
    //功能：将socket的文件描述符加入池子中，底层是红黑树维护这些文件描述符
    //核心：回调，流程是：网卡收到数据，请求硬中断，请求软中断，软中断中就会调用file_operations->poll函数，
    //poll函数动作：把事件就绪的 fd 对应的结构体放到一个特定的队列（就绪队列，ready list）；唤醒epoll_wait
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

    while (1) {
        //就绪列表中的文件描述符个数
        int nready = epoll_wait(epoll_fd, events, MAXCLIENT, -1);
        if (nready < 0) return 0;

        // events[0:nready] 是已准备好的文件描述符的 epoll_event 组成的数组。这里直接利用 events[i].data 部分
        for (int i = 0; i < nready; i++) {
            if (events[i].data.fd == socket_fd) { // 有新的 TCP 连接
                int fd = accept(socket_fd, (struct sockaddr *) &client_address, &len);
                printf("connection_fd: %d\n", fd);
                ev.data.fd = fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev); // 加入监视
            } else {
                if ((n = read(events[i].data.fd, buffer, sizeof(buffer))) > 0) {
                    printf("read: -----\n");
                    write(STDOUT_FILENO, buffer, n);
                    printf("\n-----------\n");
                    write(events[i].data.fd, buffer, n);
                } else { // read 返回零，表示 TCP 连接关闭
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &ev); // 注意：必须先退出连接，再调用 close。这与 select 和 poll 的用法不同
                    close(events[i].data.fd);
                    printf("connection_fd: %d [CLOSED]\n", events[i].data.fd);
                }
            }
        }
    }

    return 0;
}