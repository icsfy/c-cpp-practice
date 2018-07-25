#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* 每月天数 */
unsigned char days_in_month[] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/* 月份名称 */
char *month_names[12];

/* " Sun Mon Tue Wed Thu Fri Sat" */
char day_headings[29];

/* 判断闰年 */
int leap_year(unsigned yr)
{
    return (!(yr % 4) && (yr % 100)) || !(yr % 400);
}

int getdaycode(int year, int month)
{
    int daycode;
    int d1, d2, d3;
    d1 = (year - 1.) / 4.0;
    d2 = (year - 1.) / 100;
    d3 = (year - 1.) / 400;
    daycode = (year + d1 - d2 + d3) % 7;
    if (leap_year(year)) days_in_month[2] = 29;
    for (int i = 1; i < month; ++i) {
            daycode = (daycode + days_in_month[i]) % 7;
    }
    return daycode;
}

void calendar(unsigned year, unsigned month)
{
    char buf[30];
    if (month > 0 && month <= 12) {
        unsigned len;
        len = sprintf(buf, "%s %u", month_names[month - 1], year);
        printf("%*s%s\n%s\n",
            (sizeof(day_headings) - len) / 2, "", buf, day_headings);

        int daycode = getdaycode(year, month);
        /* 开始输出位置修正 */
        printf("%*s", daycode * 4, "");

        /* 循环输出天数 */
        for (int day = 1; day <= days_in_month[month]; ++day) {
            printf("%4d", day);
            if ((day + daycode) % 7 == 0) {
                putchar('\n');
            }
        }
    } else {
        perror("错误: 月份范围1~12");
        exit(-1);
    }
}

int main(int argc, char const *argv[])
{
    unsigned month = 0, year = 0, i;
    char buf[40];
    /* 从命令行解析月份和年份参数 */
    if (argv[1]) {
        if (argv[2]) {
            argv++;
            month = atoi(*argv);
        }
        argv++;
        year = atoi(*argv);
    } else {    // 无参数则取当前时间
        struct tm *ptm;
        time_t now = time(NULL);
        //puts(ctime(&now));
        ptm = localtime(&now);
        year = ptm->tm_year + 1900;
        month = ptm->tm_mon + 1;
    }
    // printf("year: %d, month: %d\n", year, month);

    memset(day_headings, ' ', sizeof(day_headings));
    day_headings[sizeof(day_headings)] = '\0';
    for (int i = 0; i < 12; ++i) {
        struct tm zero_tm;
        zero_tm.tm_mon = i;
        strftime(buf, sizeof(buf), "%B", &zero_tm);
        month_names[i] = strdup(buf);

        if (i < 7) {
            zero_tm.tm_wday = i;
            strftime(buf, sizeof(buf), "%a", &zero_tm);
            strncpy(day_headings + i * 4 + 1, buf, 3);
        }
    }
    // for (int i = 0; i < 12; ++i) {
    //     printf("%s ", month_names[i]);
    // }
    // printf("\n%s\n", day_headings);

    if (month > 0 && month < 12) {
        calendar(year, month);
    } else {
        for (month = 1; month <= 12; ++month) {
            calendar(year, month);
            if (month < 12) {
                printf("\n\n");
            }
        }
    }
    putchar('\n');
    return 0;
}
