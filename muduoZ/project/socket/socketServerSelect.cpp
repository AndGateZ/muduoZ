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

    //下面是select部分
    int client_fd[MAXCLIENT]; // connection_fd 改为数组
    bzero(client_fd, sizeof(client_fd)); // 假设不会有 fd 等于 0。实际上，0 通常是 stdin 的文件描述符

    fd_set all_set;
    FD_ZERO(&all_set);
    FD_SET(socket_fd, &all_set); // 记录 select 将监控哪些文件描述符，将socket_fd加入all_set集合
    int max_index = 0; // client_fd 数组中使用过的最大下标
    int max_client_fd = socket_fd; // 文件描述符中的最大值,目前最大就是socket_fd，没有任何连接发生

    while (1) {
        //构造一个读的数组，原因是select操作可能会改变数组，存储发生变化的文件描述符
        fd_set read_set = all_set;

        // select 函数将检查 0 至 max_client_fd 的所有文件描述符，当部分文件描述符已准备好时，select 函数将返回，并且在 read_set 记录哪些文件描述符已准备好。
        // 注意read_set 将被改变，所以有必要保留 all_set 而把 read_set 传入参数。
        // 返回值：
            //-1：发生错误，并将所有描述符集合清0，可通过errno输出错误详情。
            //0：超时。
            //正数：发生变化的文件描述符数量。
        int nready = select(max_client_fd+1, &read_set, NULL, NULL, NULL);

        // 如果socket_fd 已准备好，表示可接受新的连接
        //FD_ISSET：测试socket_fd是否在read_set集合中
        if (FD_ISSET(socket_fd, &read_set)) {
            int fd = accept(socket_fd, (struct sockaddr *) &client_address, &len);
            printf("connection_fd: %d\n", fd);
            //将接收到的client文件标识符加入到all_set集合
            FD_SET(fd, &all_set);
            //更新最大的客户端标识符的值
            if (fd > max_client_fd) {
                max_client_fd = fd;
            }
            for (int i = 0; i <= MAXCLIENT; ++i) {
                //找不到client_fd[i] == 0，意思是达到最大客户端连接数了
                if (i == MAXCLIENT) {
                    write(STDERR_FILENO, "too many clients\n", 17);
                    return 0;
                }
                //如果客户端连接数小于最大连接数，且client_fd该位置未被占用，将该连接的fd放入，并更新client_fd 数组中使用过的最大下标
                if (client_fd[i] == 0) {
                    client_fd[i] = fd;
                    if (i > max_index) max_index = i;
                    break;
                }
            }
            nready--;
        }

        // client_fd 中，有部分已准备好，可处理数据
        for (int i = 0; i <= max_index && nready > 0; ++i) {
            //碰到客户端数组中未连接的数据，跳过
            if (client_fd[i] == 0) continue;
            //如果遍历到的客户端fd在read_set里，表示client_fd已准备好发送信息
            if (FD_ISSET(client_fd[i], &read_set)) {
                if ((n = read(client_fd[i], buffer, sizeof(buffer))) > 0) {
                    printf("read: -----\n");
                    write(STDOUT_FILENO, buffer, n);
                    printf("\n-----------\n");
                    write(client_fd[i], buffer, n);
                } else {
                    close(client_fd[i]);
                    printf("connection_fd: %d [CLOSED]\n", client_fd[i]);
                    //将客户端从all_set中移除
                    FD_CLR(client_fd[i], &all_set);
                    client_fd[i] = 0;
                }
                nready--;
            }
        }
    }

    return 0;
}