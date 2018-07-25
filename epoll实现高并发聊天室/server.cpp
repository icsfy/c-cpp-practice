#include "utility.h"

int main(int argc, char const *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        printf("default server ip: %s, port: %d\n", SERVER_IP, SERVER_PORT);
    } else {
        printf("server ip: %s, port: %s\n", argv[1], argv[2]);
    }
    
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = 
        argc == 3 ? htons((uint16_t)atoi(argv[2])) : htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = 
        argc == 3 ? inet_addr(argv[1]) : inet_addr(SERVER_IP);

    // 服务端创建监听 socket
    int listener = socket(PF_INET, SOCK_STREAM, 0);
    CHECK_ERR(listener, "listener");
    printf("listen socket created\n");

    // 将服务端地址与监听 socket 绑定
    int ret = bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    CHECK_ERR(ret, "bind error");
    printf("bind success\n");

    // 开始监听
    ret = listen(listener, 5);
    CHECK_ERR(ret, "listen error");
    printf("start to listen\n");

    // 在内核中创建事件表
    int epfd = epoll_create(EPOLL_SIZE);
    CHECK_ERR(epfd, "epfd error");
    printf("epoll created, epollfd = %d\n", epfd);
    static struct epoll_event events[EPOLL_SIZE];
    // 往内核事件表里添加事件
    addfd(epfd, listener, true);
    
    while (1) {
        // epoll_events_count 表示就绪事件数目
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if (epoll_events_count < 0) {
            perror("epoll failure");
            break;
        }

        printf("epoll_events_count = %d\n", epoll_events_count);
        // 处理这 epoll_events_count 个就绪事件
        for (int i = 0; i < epoll_events_count; ++i) {
            int sockfd = events[i].data.fd;
            // 新用户连接
            if (sockfd == listener) {
                struct sockaddr_in client_address;
                socklen_t client_addrlen = sizeof(struct sockaddr_in);
                int clientfd = accept(listener, (struct sockaddr*)&client_address, &client_addrlen);

                printf("client connection from: %s:%d, clientfd = %d\n",
                    inet_ntoa(client_address.sin_addr),
                    ntohs(client_address.sin_port),
                    clientfd);

                addfd(epfd, clientfd, true);

                // 服务端用 list 保存用户连接
                clients_list.push_back(clientfd);
                printf("Add new clientfd = %d to epoll\n", clientfd);
                printf("Now there are %d clients in the chat room\n", (int)clients_list.size());

                // 服务端发送欢迎信息
                char message[BUF_SIZE];
                memset(message, 0, BUF_SIZE);
                sprintf(message, SERVER_WELCOME, clientfd);
                ret = send(clientfd, message, BUF_SIZE, 0);
                CHECK_ERR(ret, "send error");
            } else {    // 处理用户发来的消息, 并广播, 使其他用户收到消息
                ret = send_broadcast_message(sockfd);
                CHECK_ERR(ret, "error");
            }
        }
    }
    close(listener);    // 关闭 socket
    close(epfd);        // 关闭 epollfd
    return 0;
}
