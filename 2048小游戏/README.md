# 2048

通过 `ncurses` 库实现的终端下的 2048 小游戏。

## 编译运行

编译需要链接 `ncurses` 库:

```shell
$ g++ -o 2048 2048.c -lncurses
```

运行游戏:

```shell
$ ./2048
```

## 游戏说明

可以通过上下左右或者 `w` `s` `a` `d` 按键进行控制

`r` 键可以重新开始游戏, `q` 键退出游戏

游戏默认 `4 x 4` 方格, 可以通过命令行参数指定 `n x n` 的方格

例如

```shell


                                  --- 2048 ---

                            ┌─────┬─────┬─────┬─────┐ score:    0
                            │     │     │     │     │
                            │     │     │     │     │
                            │     │     │     │     │
                            ├─────┼─────┼─────┼─────┤
                            │     │     │     │     │
                            │     │  2  │     │     │
                            │     │     │     │     │
                            ├─────┼─────┼─────┼─────┤
                            │     │     │     │     │
                            │  4  │     │     │     │
                            │     │     │     │     │
                            ├─────┼─────┼─────┼─────┤
                            │     │     │     │     │
                            │     │     │     │     │ 'r': Restart
                            │     │     │     │     │ 'q': Quit
                            └─────┴─────┴─────┴─────┘



```

```shell
$ ./2048 5
                                  --- 2048 ---

                         ┌─────┬─────┬─────┬─────┬─────┐ score:         0
                         │     │     │     │     │     │
                         │     │     │     │     │     │
                         │     │     │     │     │     │
                         ├─────┼─────┼─────┼─────┼─────┤
                         │     │     │     │     │     │
                         │     │     │     │     │     │
                         │     │     │     │     │     │
                         ├─────┼─────┼─────┼─────┼─────┤
                         │     │     │     │     │     │
                         │     │     │     │     │     │
                         │     │     │     │     │     │
                         ├─────┼─────┼─────┼─────┼─────┤
                         │     │     │     │     │     │
                         │     │     │     │  4  │     │
                         │     │     │     │     │     │
                         ├─────┼─────┼─────┼─────┼─────┤
                         │     │     │     │     │     │
                         │     │  2  │     │     │     │ 'r': Restart
                         │     │     │     │     │     │ 'q': Quit
                         └─────┴─────┴─────┴─────┴─────┘

```

## 游戏结束

```


                                  --- 2048 ---

                            ┌─────┬─────┬─────┬─────┐ score:    5720
                            │     │     │     │     │ Best:     5720
                            │  4  │  8  │  4  │  2  │
                            │     │     │     │     │
                            ├─────┼─────┼─────┼─────┤
                            │     │     │     │     │
                            │  8  │  32 │  16 │  32 │
                            │     │     │     │     │
                            ├─────┼─────┼─────┼─────┤
                            │     │     │     │     │
                            │  64 │ 512 │ 128 │  64 │
                            │     │     │     │     │
                            ├─────┼─────┼─────┼─────┤
                            │     │     │     │     │
                            │  2  │  4  │  8  │  2  │ 'r': Restart
                            │     │     │     │     │ 'q': Quit
                            └─────┴─────┴─────┴─────┘
                                   GAME OVER!


```