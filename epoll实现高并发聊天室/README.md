# epoll 实现高并发聊天室

## Server

```shell
$ g++ server.cpp -o server
```

```shell
$ ./server 127.0.0.1 8888
server ip: 127.0.0.1, port: 8888
listen socket created
bind success
start to listen
epoll created, epollfd = 4
fd added to epoll!


```

## Client

```shell
$ g++ client.cpp -o client
```

```shell
$ ./client 127.0.0.1 8888
connect to 127.0.0.1:8888
fd added to epoll!

fd added to epoll!

Welcome you join to the chat room! Your chat ID is: Client #5

>> You can input exit to exit the chat room
>> 
```
