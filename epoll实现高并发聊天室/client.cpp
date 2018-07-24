#include "utility.h"

int main(int argc, char const *argv[])
{
    printf("server ip: %s\n", SERVER_IP);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 创建 socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    CHECK_ERR(sock, "sock error");

    // 连接服务器
    int ret = connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    CHECK_ERR(ret, "connect error");

    // 创建管道, 其中 fd[0] 用于父进程读, fd[1]用于子进程写
    int pipe_fd[2];
    ret = pipe(pipe_fd);
    CHECK_ERR(ret, "pipe error");

    // 创建 epoll
    int epfd = epoll_create(EPOLL_SIZE);
    CHECK_ERR(epfd, "epfd error");
    static struct epoll_event events[2];
    // 将 sock 和管道读端描述符都添加到内核事件表中
    addfd(epfd, sock, true);
    addfd(epfd, pipe_fd[0], true);

    // 表示客户端是否正常工作
    bool isClientwork = true;

    // 聊天信息缓冲区
    char message[BUF_SIZE];

    int pid = fork();
    CHECK_ERR(pid, "fork error");
    if (pid == 0) { // 子进程
        // 子进程负责写入管道, 因此先关闭读端
        close(pipe_fd[0]);
        printf("You can input " EXIT " to exit the chat room\n");

        while (isClientwork) {
            memset(message, 0, BUF_SIZE);
            fgets(message, BUF_SIZE, stdin);

            // 客户输入 exit 退出
            if (strncasecmp(message, EXIT, strlen(EXIT)) == 0) {
                isClientwork = 0;
            } else {    // 子进程将信息写入管道
                ret = write(pipe_fd[1], message, strlen(message) -1);
                CHECK_ERR(ret, "write error");
                PRINT_HINT();
            }
        }
    } else {    // pid > 0 父进程
        // 父进程负责读取管道数据, 因此先关闭写端
        close(pipe_fd[1]);

        while (isClientwork) {
            int epoll_events_count = epoll_wait(epfd, events, 2, -1);
            // 处理就绪事件
            for (int i = 0; i < epoll_events_count; ++i) {
                memset(message, 0, BUF_SIZE);

                // 服务端发来消息
                if (events[i].data.fd == sock) {
                    // 接收服务端消息
                    ret = recv(sock, message, BUF_SIZE, 0);
                    // ret = 0 服务端关闭
                    if (ret == 0) {
                        printf("server closed connection: %d\n", sock);
                        close(sock);
                        isClientwork = 0;
                    } else {
                        printf("%s\n", message);
                        PRINT_HINT();
                    }
                } else {    // 子进程写入事件发生, 父进程处理并发送至服务端
                    ret = read(events[i].data.fd, message, BUF_SIZE);

                    if (ret == 0) {
                        isClientwork = 0;
                    } else {
                        send(sock, message, BUF_SIZE, 0);
                    }
                }
            }
        }
    }

    if (pid) {
        // 关闭父进程和sock
        close(pipe_fd[0]);
        close(sock);
    } else {
        // 关闭子进程
        close(pipe_fd[1]);
    }

    return 0;
}
