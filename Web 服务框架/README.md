# C++ 开发 Web 服务框架

- C++ 11
- Boost Asio

## Compile

```shell
$ cmake .
$ make
```

## Run

### Run main_http

```shell
$ ./main_http

```

curl Test:

```shell
$ curl localhost:12345
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Web Framework Test</title>
</head>
<body>
<h1>Hello world in index.html.</h1>
</body>
</html>%
$ curl -d "hello, world\!" localhost:12345/string
hello, world!%
$ curl localhost:12345/match/123abc
123abc%
```

### Run main_https

```shell
$ ./main_https

```

curl Test:

```shell
$ curl https://localhost:12345 
curl: (60) SSL certificate problem: self signed certificate
More details here: https://curl.haxx.se/docs/sslcerts.html

curl failed to verify the legitimacy of the server and therefore could not
establish a secure connection to it. To learn more about this situation and
how to fix it, please visit the web page mentioned above.
```

```shell
$ curl -k https://localhost:12345
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Web Framework Test</title>
</head>
<body>
<h1>Hello world in index.html.</h1>
</body>
</html>%
```