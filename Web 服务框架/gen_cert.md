# 创建证书文件

## 生成私钥

```shell
$ openssl genrsa -des3 -out server.key 1024
```

## 生成 CSR

```shell
$ openssl req -new -key server.key -out server.csr
```

## 从秘钥中移除密码

```shell
$ cp server.key server.key.old
$ openssl rsa -in server.key.old -out server.key
```

## 生成自签名证书

```shell
$ openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt
```
