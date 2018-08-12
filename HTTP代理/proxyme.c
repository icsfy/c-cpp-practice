#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>
#include <stdio.h>
#include <arpa/inet.h>

#define SERVER_NAME             "ProxyMe"
#define SERVER_VERSION          "v0.1"

#define DEFAULT_LOCAL_PORT      8090
#define DEFAULT_REMOTE_PORT     8091

#define MAX_HEADER_SIZE         8192
#define BUF_SIZE                16384

#define LOG(fmt...)     do { fprintf(stderr, ##fmt); } while(0)

#define CHECK_ERR(val, msg)  do { if ((val) < 0) { perror(msg); return -1; } } while(0)

/* 从命令行解析的字段 */
uint16_t local_port = DEFAULT_LOCAL_PORT;
char remote_host[128];
uint16_t remote_port = DEFAULT_REMOTE_PORT;
int run_as_daemon = 0;

int server_sock;

/* 读取 HTTP 头部, 遇到空行结束 */
int read_header(int fd, char *buffer, size_t n) {
    char ch;
    ssize_t stat;
    for (int i = 0; i < n; ++i) {
        stat = recv(fd, &ch, 1, 0);
        if (stat > 0)
            buffer[i] = ch;
        else if (stat == 0) {
            errno = ECONNRESET;
            return -1;
        } else
            return -1;
        if (ch == '\n') {
            if (i > 0 && buffer[i - 1] == '\n')   /* "\n\n" */
                return i;
            if (i > 2 && buffer[i - 3] == '\r' && buffer[i - 2] == '\n' && buffer[i - 1] == '\r')     /* "\r\n\r\n" */
                return i;
        }
    }
    errno = ENOBUFS;
    return -1;
}

int extract_host(const char *header, char *host, uint16_t *port) {
    *port = 80;
    char *p = strstr(header, "CONNECT");    /* 在 CONNECT 方法中解析隧道主机名称及端口号 */
    if (p) {                                /* "CONNECT www.example.com:443 HTTP/1.1" */
        char *p1 = strchr(p, ' ');          /* p1指向第一处空格: "_www.exa..." */
        char *p2 = strchr(p1 + 1, ':');     /* p2指向冒号: ":443..." */
        char *p3 = strchr(p1 + 1, ' ');     /* p3指向第二处空格: "_HTTP..." */

        if (p2) {                                   /* 有冒号则可以解析出host和端口号 */
            strncpy(host, p1 + 1, p2 - (p1 + 1));
            *port = (uint16_t) atoi(p2 + 1);
        } else {
            strncpy(host, p1 + 1, p3 - (p1 + 1));
        }
        return 0;
    }
    /* 没有"CONNECT"则通过"Host:"字段解析 */
    p = strstr(header, "Host:");            /* "Host: server.example.com:80" */
    if (!p) {
        return -1;
    }
    char *p1 = strchr(p, '\n');             /* p1指向host行末尾换行符 */
    if (!p1) {
        return -1;
    }
    char *p2 = strchr(p + 5, ':');          /* 寻找“Host:”后面冒号: ":80" */
    if (p2 && p2 < p1) {
        strncpy(host, p + 5 + 1, p2 - (p + 5 + 1));
        *port = (uint16_t) atoi(p2 + 1);
    } else {
        strncpy(host, p + 5 + 1, (p1 - 1) - (p + 5 + 1));
    }
    return 0;
}

/* 响应隧道连接请求 */
int send_tunnel_ok(int sock) {
    char *resp = "HTTP/1.1 200 Connection Established\r\n\r\n";
    CHECK_ERR(send(sock, resp, strlen(resp), 0), "send_tunnel_ok");
    return 0;
}

/* 获取运行信息 */
void get_info(char *output) {
    int pos = 0;
    pos += sprintf(output, "====== " SERVER_NAME "(" SERVER_VERSION ") ======\n");
    if (strlen(remote_host) != 0)
        pos += sprintf(output + pos, ">>> data will be sent to the remote server %s:%d\n", remote_host, remote_port);
    sprintf(output + pos, ">>> server listen on local port: %d\n", local_port);
}

/* 重写 HTTP 头部请求路径, 处理以下两种情况
 * - "GET http://example.com/abc HTTP/1.1" 被转换为 "GET /abc HTTP/1.1"
 * - "GET http://example.com HTTP/1.1" 被转换为 "GET / HTTP/1.1" */
void rewrite_header(char *header) {
    char *p_http = strstr(header, "http://");
    if (p_http) {
        char *p_end = strchr(p_http, '\0');
        char *p_slash = strchr(p_http + 7, '/');
        char *p_http_1_1 = strstr(header, "HTTP/");
        if (p_slash < p_http_1_1) {
            memcpy(p_http, p_slash, (p_end - p_slash) + 1);
        } else {
            *p_http = '/';
            memcpy(p_http + 1, p_http_1_1 - 1, p_end - (p_http_1_1 - 1) + 1);
        }
    }
}

void forward_header(int sock, char *header) {
    rewrite_header(header);
#ifdef DEBUG
    LOG(">>>forward_header:\n%s", header);
#endif
    send(sock, header, strlen(header), 0);
}

void forward_data(int source_sock, int destination_sock) {
    char buffer[BUF_SIZE];
    int n;
    while ((n = (int) recv(source_sock, buffer, sizeof(buffer), 0)) > 0) {
        send(destination_sock, buffer, (size_t) n, 0);
    }
    shutdown(destination_sock, SHUT_RDWR);
    shutdown(source_sock, SHUT_RDWR);
}

int create_connection(const char *host, uint16_t port) {
    struct sockaddr_in server_addr;
    struct hostent *server;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_ERR(sock, "socket");

    if ((server = gethostbyname(host)) == NULL) {
        herror(host);
        errno = EFAULT;
        return -1;
    }
#ifdef DEBUG
    LOG("create_connection: [%s:%d]\n", host, port);
#endif
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&(server_addr.sin_addr.s_addr), server->h_addr, (size_t) server->h_length);
    server_addr.sin_port = htons(port);

    CHECK_ERR(connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)), "connect");
    return sock;
}

/* 处理客户端的连接 */
int handle_client(int client_sock) {
    int is_http_tunnel = 0;
    int remote_sock;
    uint16_t port = remote_port;

    char header[MAX_HEADER_SIZE];
    memset(header, 0, MAX_HEADER_SIZE);
    if (strlen(remote_host) == 0) {             /* 未指定 remote_host, 从 HTTP 请求解析 HOST */
        CHECK_ERR(read_header(client_sock, header, sizeof(header)), "read_header");
#ifdef DEBUG
        LOG(">>>Header: \n%s", header);
#endif
        if (strstr(header, "CONNECT")) {        /* HTTP 隧道请求 */
#ifdef DEBUG
            LOG("receive CONNECT request\n");
#endif
            is_http_tunnel = 1;
        }
        if (strstr(header, "GET /")) {
            return -1;
        }

        char host[512];
        memset(host, 0, sizeof(host));
        if (extract_host(header, host, &port) < 0) {
            LOG("Cannot extract host field, bad http protocol");
            return -1;
        }

#ifdef DEBUG
        LOG("handle_client: [%s:%d]\n", host, port);
#endif
        remote_sock = create_connection(host, port);
    } else {
        remote_sock = create_connection(remote_host, port);
    }
    CHECK_ERR(remote_sock, "create_connection");

    if (fork() == 0) {      /* 创建子进程用于从客户端转发数据到远端 socket 接口 */
        if (strlen(header) > 0 && !is_http_tunnel) {
            forward_header(remote_sock, header);
        }
        forward_data(client_sock, remote_sock);
        exit(0);
    }

    if (fork() == 0) {      /* 创建子进程用于接收远端 socket 接口的数据到客户端 */
        if (is_http_tunnel) {
            send_tunnel_ok(client_sock);
        }
        forward_data(remote_sock, client_sock);
        exit(0);
    }

    close(remote_sock);
    close(client_sock);
    return 0;
}

void server_loop() {
    for (;;) {
        int client_sock = accept(server_sock, NULL, NULL);
        if (fork() == 0) {      // 创建子进程处理客户端连接请求
            close(server_sock);
            if (handle_client(client_sock) < 0)
                close(client_sock);
            exit(0);
        }
        close(client_sock);
    }
}

int create_server_socket(uint16_t local_port) {
    int server_sock, optval = 1;
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(local_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_ERR(server_sock, "socket");

    CHECK_ERR(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)), "setsockopt");

    CHECK_ERR(bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)), "bind");

    CHECK_ERR(listen(server_sock, 20), "listen");

    return server_sock;
}

/* 防止子进程结束后成为僵尸进程 */
void sigchld_handler(int signal) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void sigterm_handler(int signal) {
    close(server_sock);
    exit(0);
}

void usage() {
    printf("\n"SERVER_NAME" - "SERVER_VERSION);
    printf("\nUsage:\n"
           "  [-d]                  Run as daemon(default: no).\n"
           "  [-l <local_port>]     Port number of your local server(default: %d).\n"
           "  [-h <remote_host>]    Host name or IP address of your remote server.\n"
           "  [-p <remote_port>]    Port number of your remote server(default: %d).\n",
           DEFAULT_LOCAL_PORT, DEFAULT_REMOTE_PORT);
    exit(0);
}

/* 解析命令行参数 */
void parse_options(int argc, char *argv[]) {
    int opt;
    while (-1 != (opt = getopt(argc, argv, ":l:h:p:d"))) {
        switch (opt) {
            case 'l':
                local_port = (uint16_t) atoi(optarg);
                break;
            case 'h':
                strncpy(remote_host, optarg, strlen(optarg));
                break;
            case 'p':
                remote_port = (uint16_t) atoi(optarg);
                break;
            case 'd':
                run_as_daemon = 1;
                break;
            case ':':
                printf("Missing argument after: -%c\n", optopt);
                usage();
            case '?':
                printf("Invalid argument: %c\n", optopt);
            default:
                usage();
        }
    }
}

int main(int argc, char *argv[]) {
    parse_options(argc, argv);

    {   /* 输出运行信息 */
        char server_info[1024];
        get_info(server_info);
        LOG("%s\n", server_info);
    }

    signal(SIGCHLD, sigchld_handler);
    signal(SIGTERM, sigterm_handler);

    server_sock = create_server_socket(local_port);
    CHECK_ERR(server_sock, "create_server_socket");

    if (run_as_daemon) {
        pid_t pid;
        CHECK_ERR(pid = fork(), "fork");
        if (pid == 0) {
            server_loop();  // 子进程作为 daemon 运行
        }
        LOG("proxy child pid: [%d]\n", pid);
        close(server_sock);
    } else {
        server_loop();      // 不以 daemon 方式运行
    }
    return 0;
}
