# HTTP 代理

A simple http proxy.

## 编译

```shell
$ gcc proxyme.c -o proxyme
```

或者 `DEBUG` 显示更多信息

```shell
$ gcc -DDEBUG -o proxyme proxyme.c
```

## 启动

```shell
ProxyMe - v0.1
Usage:
  [-d]                  Run as daemon(default: no).
  [-l <local_port>]     Port number of your local server(default: 8090).
  [-h <remote_host>]    Host name or IP address of your remote server.
  [-p <remote_port>]    Port number of your remote server(default: 8091).
```

指定 `-d` 选项将以 `daemon` 方式运行:

```shell
$ ./proxyme -d
====== ProxyMe(v0.1) ======
>>> server listen on local port: 8090

proxy child pid: [22764]
$ ps  
  PID TTY          TIME CMD
21784 pts/2    00:00:00 zsh
22764 pts/2    00:00:00 proxyme
22793 pts/2    00:00:00 ps
$ kill 22764
$ ps
  PID TTY          TIME CMD
21784 pts/2    00:00:00 zsh
22810 pts/2    00:00:00 ps
```

直接本地代理:

```shell
$ ./proxyme -l 8090
====== ProxyMe(v0.1) ======
>>> server listen on local port: 8090


```

或者本地连接到远端服务器, 通过远端服务器代理:

```shell
# 假设远端 IP: 192.168.1.100 端口: 8091
# 远端启动
$ ./proxyme -l 8091
====== ProxyMe(v0.1) ======
>>> server listen on local port: 8091

# 本地启动(连接到远端, 本地监听端口: 8090)
$ ./proxyme -h 192.168.1.100 -p 8091 -l 8090
====== ProxyMe(v0.1) ======
>>> data will be sent to the remote server 192.168.1.100:8091
>>> server listen on local port: 8090

```

## 测试连接

```shell
$ http_proxy=http://127.0.0.1:8090 curl baidu.com
<html>
<meta http-equiv="refresh" content="0;url=http://www.baidu.com/">
</html>
```

## 为浏览器设置代理

`Firfox` 设置代理: `首选项` -- `常规` -- `网络代理` -- `设置` -- `手动代理配置`

- `HTTP 代理`: `127.0.0.1`
- `端口`: `8090`
- [x] `为所有协议使用相同代理服务器`
- `确定`

`Chrome` 通过命令行启动设置代理:

```shell
$ google-chrome-stable --proxy-server="http://127.0.0.1:8090"
```

:-)
