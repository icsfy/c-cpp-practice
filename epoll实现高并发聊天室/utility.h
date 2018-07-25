#ifndef __UTILITY__
#define __UTILITY__

#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 服务端存储所有在线用户 socket, 便于广播信息
std::list<int> clients_list;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

#define EPOLL_SIZE 5000

// 消息缓冲区大小
#define BUF_SIZE 0xffff

#define SERVER_WELCOME "Welcome you join to the chat room! Your chat ID is: Client #%d\n"
#define SERVER_MESSAGE "\nClientID #%d say >> %s"
#define EXIT "exit"
#define CAUTION "\nThere is only one in the chat room!\n"

#define CHECK_ERR(err, msg) do {\
                                if (err < 0) {\
                                    perror(msg);\
                                    exit(-1);\
                                }} while(0)

#define PRINT_HINT() do {\
                        printf(">> ");\
                        fflush(stdout);\
                    } while(0)

// 将文件描述符 fd 设置为非阻塞方式
int setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

// 将文件描述符 fd 添加到 epollfd 标示的内核事件表中
void addfd(int epollfd, int fd, bool enable_et)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et) {
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setnonblocking(fd);
    printf("fd added to epoll!\n\n");
}

// 将服务器收到的 clientfd 的消息进行广播
int send_broadcast_message(int clientfd)
{
    char buf[BUF_SIZE], message[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    memset(message, 0, BUF_SIZE);

    printf("read from client(clientID: %d)\n", clientfd);
    int len = recv(clientfd, buf, BUF_SIZE, 0);

    if (len == 0) { // len = 0 表明客户端关闭了连接
        close(clientfd);
        clients_list.remove(clientfd);  // 移除客户端
        printf("ClientID: %d closed.\n"
            "now there are %d client in the chat room\n",
            clientfd, (int)clients_list.size());
    } else {    // 广播消息
        if (clients_list.size() == 1) { // 只有一个客户端连接
            send(clientfd, CAUTION, strlen(CAUTION), 0);
            return len;
        }

        sprintf(message, SERVER_MESSAGE, clientfd, buf);

        std::list<int>::iterator it;
        for (it = clients_list.begin(); it != clients_list.end(); ++it) {
            if (*it != clientfd) {
                int ret = send(*it, message, BUF_SIZE, 0);
                CHECK_ERR(ret, "error");
            }
        }
    }
    return len;
}

#endif /* __UTILITY__ */
