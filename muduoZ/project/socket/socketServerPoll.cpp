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

    //下面是poll部分
    //struct pollfd
    //{
    //int fd;			/* File descriptor to poll.  */
    //short int events;		/* Types of events poller cares about.  */
    //short int revents;		/* Types of events that actually occurred.  */
    //};
    struct pollfd client_pollfd[MAXCLIENT];
    bzero(client_pollfd, sizeof(client_pollfd));

    int max_index = 0;

    //第一个是监视描述符
    client_pollfd[0].fd = socket_fd;
    client_pollfd[0].events = POLLIN; // events 记录当前文件描述符需要监控哪些内容，POLLIN 代表监控可读内容

    while (1) {
        //返回文件描述符变化个数
        int nready = poll(client_pollfd, max_index+1, -1);
        //第一个是监视描述符
        if (client_pollfd[0].revents & POLLIN) { // 监控结果放在 revents 内。与 select 函数不同，监控结果不会覆盖原来的参数
            int fd = accept(socket_fd, (struct sockaddr *) &client_address, &len);
            printf("connection_fd: %d\n", fd);

            for (int i = 0; i <= MAXCLIENT; ++i) {
                if (i == MAXCLIENT) {
                    write(STDERR_FILENO, "too many clients\n", 17);
                    return 0;
                }
                if (client_pollfd[i].fd <= 0) {
                    client_pollfd[i].fd = fd;
                    client_pollfd[i].events = POLLIN;
                    if (i > max_index) max_index = i;
                    break;
                }
            }
            nready--;
        }
        //处理client的描述符操作
        for (int i = 0; i <= max_index && nready > 0; ++i) {
            if (client_pollfd[i].fd <= 0) continue;
            if (client_pollfd[i].revents & POLLIN) {
                if ((n = read(client_pollfd[i].fd, buffer, sizeof(buffer))) > 0) {
                    printf("read: -----\n");
                    write(STDOUT_FILENO, buffer, n);
                    printf("\n-----------\n");
                    write(client_pollfd[i].fd, buffer, n);
                } else {
                    close(client_pollfd[i].fd);
                    printf("connection_fd: %d [CLOSED]\n", client_pollfd[i].fd);
                    client_pollfd[i].fd = -1; // 负的文件描述符表示 poll 函数将忽略当前项
                }
                nready--;
            }
        }
    }

    return 0;
}