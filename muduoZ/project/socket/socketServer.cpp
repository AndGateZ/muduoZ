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

int main(void) {
    pid_t pid = getpid();
    printf("pid: %d\n", pid);

    //初始化获得一个socket监听套接字的文件描述符，存储于/proc/<pid>/fd/文件夹下
    // { * : 9999, * : * }
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    printf("socket_fd: %d\n", socket_fd);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9999);
    
    bind(socket_fd, (const struct sockaddr *) &server_address, sizeof(server_address));
    listen(socket_fd, LISTENQ);

    struct sockaddr_in client_address;
    char buffer[MAXLINE];
    ssize_t n;
    socklen_t len = sizeof(client_address);

    while (1) {
        //获得具体 TCP 连接的四元组，每当 socket_fd 接受一个连接，它将会产生一个 connection_fd，并由它来处理数据传输
        // { 192.168.1.11 : 9999, 192.168.1.12 : 36500 }{目的IP：目的端口，发送IP：发送端口}
        //服务端接收到任何来自192.168.1.12 : 36500，目的为192.168.1.11 : 9999的TCP分节，将其与connection_fd关联
        //服务端收到任何目的端口为9999的TCP分节，将其与socket_fd关联
        int connection_fd = accept(socket_fd, (struct sockaddr *) &client_address, &len);
        printf("connection_fd: %d\n", connection_fd);
        while ((n = read(connection_fd, buffer, sizeof(buffer))) > 0) {
            printf("read: -----\n");
            write(STDOUT_FILENO, buffer, n);
            printf("\n-----------\n");
            write(connection_fd, buffer, n);
        }
        close(connection_fd);
        printf("connection_fd: %d [CLOSED]\n", connection_fd);
    }

    return 0;
}