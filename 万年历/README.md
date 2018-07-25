# 显示一个日历

显示一个简单的日历..  如果没有指定参数, 则显示当前月份.

参数如果是 `[ 年份 ]`, 则输出该年所有月份的日历.

参数如果是 `[ 月份 ] [ 年份 ]`, 则输出指定月份的日历.

## 编译

```shell
$ g++ calendar.c -o calendar
```

## 示例

### 示例1. 无参数

```shell
$ ./calendar
          July 2018
 Sun Mon Tue Wed Thu Fri Sat 
   1   2   3   4   5   6   7
   8   9  10  11  12  13  14
  15  16  17  18  19  20  21
  22  23  24  25  26  27  28
  29  30  31
```

### 示例2. 参数为月份+年份

```shell
$ ./calendar 8 2018
         August 2018
 Sun Mon Tue Wed Thu Fri Sat 
               1   2   3   4
   5   6   7   8   9  10  11
  12  13  14  15  16  17  18
  19  20  21  22  23  24  25
  26  27  28  29  30  31
```

### 示例3. 参数为年份

```shell
$ ./calendar 2018
        January 2018
 Sun Mon Tue Wed Thu Fri Sat 
       1   2   3   4   5   6
   7   8   9  10  11  12  13
  14  15  16  17  18  19  20
  21  22  23  24  25  26  27
  28  29  30  31

        February 2018
 Sun Mon Tue Wed Thu Fri Sat 
                   1   2   3
   4   5   6   7   8   9  10
  11  12  13  14  15  16  17
  18  19  20  21  22  23  24
  25  26  27  28

         March 2018
 Sun Mon Tue Wed Thu Fri Sat 
                   1   2   3
   4   5   6   7   8   9  10
  11  12  13  14  15  16  17
  18  19  20  21  22  23  24
  25  26  27  28  29  30  31


         April 2018
 Sun Mon Tue Wed Thu Fri Sat 
   1   2   3   4   5   6   7
   8   9  10  11  12  13  14
  15  16  17  18  19  20  21
  22  23  24  25  26  27  28
  29  30

          May 2018
 Sun Mon Tue Wed Thu Fri Sat 
           1   2   3   4   5
   6   7   8   9  10  11  12
  13  14  15  16  17  18  19
  20  21  22  23  24  25  26
  27  28  29  30  31

          June 2018
 Sun Mon Tue Wed Thu Fri Sat 
                       1   2
   3   4   5   6   7   8   9
  10  11  12  13  14  15  16
  17  18  19  20  21  22  23
  24  25  26  27  28  29  30


          July 2018
 Sun Mon Tue Wed Thu Fri Sat 
   1   2   3   4   5   6   7
   8   9  10  11  12  13  14
  15  16  17  18  19  20  21
  22  23  24  25  26  27  28
  29  30  31

         August 2018
 Sun Mon Tue Wed Thu Fri Sat 
               1   2   3   4
   5   6   7   8   9  10  11
  12  13  14  15  16  17  18
  19  20  21  22  23  24  25
  26  27  28  29  30  31

       September 2018
 Sun Mon Tue Wed Thu Fri Sat 
                           1
   2   3   4   5   6   7   8
   9  10  11  12  13  14  15
  16  17  18  19  20  21  22
  23  24  25  26  27  28  29
  30

        October 2018
 Sun Mon Tue Wed Thu Fri Sat 
       1   2   3   4   5   6
   7   8   9  10  11  12  13
  14  15  16  17  18  19  20
  21  22  23  24  25  26  27
  28  29  30  31

        November 2018
 Sun Mon Tue Wed Thu Fri Sat 
                   1   2   3
   4   5   6   7   8   9  10
  11  12  13  14  15  16  17
  18  19  20  21  22  23  24
  25  26  27  28  29  30

        December 2018
 Sun Mon Tue Wed Thu Fri Sat 
                           1
   2   3   4   5   6   7   8
   9  10  11  12  13  14  15
  16  17  18  19  20  21  22
  23  24  25  26  27  28  29
  30  31
```
