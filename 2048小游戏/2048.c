#include "2048.h"

/* 游戏结束标志 */
int gameoverflag;

/* 游戏分数 */
int score, topscore;

int main(int argc, char const *argv[])
{
    int **arr, n = 4, ch, moved;
    if (argc == 2)
        n = atoi(argv[1]);
    topscore = 0;
LABEL_RESTART:
    gameoverflag = 0;
    score = 0;
    arr = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; ++i)
        arr[i] = (int *)calloc(n, sizeof(int));
    srand(time(NULL));
    initscr();
    curs_set(0);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    update2048(arr, n, 2);
    print2048(arr, n);
    while ((ch = getch()) != GAME_KEY_EXIT)
    {
        switch (ch)
        {
            case KEY_LEFT:
            case GAME_KEY_LEFT:
                moved = move_left_right(arr, n, GAME_KEY_LEFT);
                break;
            case KEY_RIGHT:
            case GAME_KEY_RIGHT:
                moved = move_left_right(arr, n, GAME_KEY_RIGHT);
                break;
            case KEY_UP:
            case GAME_KEY_UP:
                moved = move_up_down(arr, n, GAME_KEY_UP);
                break;
            case KEY_DOWN:
            case GAME_KEY_DOWN:
                moved = move_up_down(arr, n, GAME_KEY_DOWN);
                break;
            case GAME_KEY_RESTART:
                goto FREE_RESOURCE;
                break;
        }
        /* 移动了则更新, 没移动则提示不能移动 */
        if (moved) {
            update2048(arr, n, 1);
            check2048(arr, n);
            print2048(arr, n);
        } else if (!gameoverflag) {
            attron(A_BLINK);
            mvprintw(0, (COLS - sizeof(GAME_TEXT_NOTMOVE)) / 2, GAME_TEXT_NOTMOVE);
            attroff(A_BLINK);
        }
    }
FREE_RESOURCE:
    endwin();
    /* 释放资源 */
    for (int i = 0; i < n; ++i)
        free(arr[i]);
    free(arr);
    /* 判断是否重新开始游戏 */
    if (ch == GAME_KEY_RESTART)
        goto LABEL_RESTART;
    return 0;
}

/* 输出方框边界 */
void board(WINDOW *win, int starty, int startx, int lines, int cols,
           int tile_width, int tile_height)
{
    int endy, endx, i, j;
    endy = starty + lines * tile_height;
    endx = startx + cols * tile_width;

    for (j = starty; j <= endy; j += tile_height)
        for (i = startx; i < endx; ++i)
            mvwaddch(win, j, i, ACS_HLINE);
    for (i = startx; i <= endx; i += tile_width)
        for (j = starty; j <= endy; ++j)
            mvwaddch(win, j, i, ACS_VLINE);
    mvwaddch(win, starty, startx, ACS_ULCORNER);
    mvwaddch(win, endy, startx, ACS_LLCORNER);
    mvwaddch(win, starty, endx, ACS_URCORNER);
    mvwaddch(win, endy, endx, ACS_LRCORNER);
    for (j = starty + tile_height; j <= endy - tile_height; j += tile_height)
    {
        mvwaddch(win, j, startx, ACS_LTEE);
        mvwaddch(win, j, endx, ACS_RTEE);
        for (i = startx + tile_width; i <= endx - tile_width; i += tile_width)
            mvwaddch(win, j, i, ACS_PLUS);
    }
    for (i = startx + tile_width; i <= endx - tile_width; i += tile_width)
    {
        mvwaddch(win, starty, i, ACS_TTEE);
        mvwaddch(win, endy, i, ACS_BTEE);
    }
    wrefresh(win);
}

/* 检查是否能继续游戏 设置游戏结束标志位 */
void check2048(int **arr, int n)
{
    int x, y;
    for (x = 0; x < n; ++x) {
        for (y = 0; y < n; ++y) {
            if (arr[x][y] == 0) return;
            if (x + 1 < n && arr[x+1][y] == arr[x][y]) return;
            if (y + 1 < n && arr[x][y+1] == arr[x][y]) return;
        }
    }
    gameoverflag = 1;
}

/* 随机生成数字2或4, count指定生成数字个数 */
void update2048(int **arr, int n, int count)
{
    int x, y, num, i, position;
    num = 0;
    /* 获取空格数 */
    for (x = 0; x < n; ++x)
        for (y = 0; y < n; ++y)
            if (arr[x][y] == 0)
                num++;
    while (count > 0 && num > 0)
    {
        /* 数字放置位置 */
        position = rand() % num;
        i = 0;
        for (x = 0; x < n && i <= position; ++x)
            for (y = 0; y < n && i <= position; ++y)
                if (arr[x][y] == 0)
                    if (i++ == position)
                        arr[x][y] = rand() % 100 < PERCENT_2 ? 2 : 4;
        count--;
        num--;
    }
}

/* 左右移动合并 */
int move_left_right(int **arr, int n, int direction)
{
    int x, y, k, delta;
    int moved = 0;
    /* 一行一行处理 */
    for (y = 0; y < n; ++y)
    {
        if (direction == GAME_KEY_LEFT) {
            /* 向左滑动 从左向右处理 */
            k = 0;  x = 1;  delta = 1;
        } else if (direction == GAME_KEY_RIGHT) {
            /* 向右滑动 从右向左处理 */
            k = n - 1;  x = n - 2;  delta = -1;
        }
        while ((direction == GAME_KEY_LEFT && x < n) ||
                (direction == GAME_KEY_RIGHT && x >= 0))
        {
            if (arr[x][y] != 0) {   /* 当前格不为空 */
                if (arr[k][y] == 0) {       /* 处理格为空 */
                    arr[k][y] = arr[x][y];  // 滑至处理格
                    arr[x][y] = 0;          // 当前格置空
                    x += delta;             // 当前格指向下一格
                    moved = 1;
                } else {                    /* 处理格不为空 */
                    if (arr[x][y] == arr[k][y]) {   /* 可以合并 */
                        arr[k][y] += arr[x][y];     // 合并当前格
                        arr[x][y] = 0;              // 当前格置空
                        score += arr[k][y];         // 分数加上合并值
                        x += delta;                 // 当前格指向下一格
                        k += delta;                 // 处理格指向下一格
                        moved = 1;
                    } else {                        /* 不能合并 */
                        k += delta;                 // 处理格指向下一格
                        if (k == x)                 // 若处理格指向当前格
                            x += delta;             // 则当前格指向下一格
                    }
                }
            } else {                /* 当前格为空 */
                x += delta;         // 当前格指向下一格
            }
        }
    }
    return moved;
}

/* 上下移动合并 */
int move_up_down(int **arr, int n, int direction)
{
    int x, y, k, delta;
    int moved = 0;
    /* 一列一列处理 */
    for (x = 0; x < n; ++x)
    {
        if (direction == GAME_KEY_UP) {
            /* 向上滑动 从上向下处理 */
            k = 0;  y = 1;  delta = 1;
        } else if (direction == GAME_KEY_DOWN) {
            /* 向下滑动 从下向上处理 */
            k = n - 1;  y = n - 2;  delta = -1;
        }
        while ((direction == GAME_KEY_UP && y < n) ||
                (direction == GAME_KEY_DOWN && y >= 0))
        {
            if (arr[x][y] != 0) {   /* 当前格不为空 */
                if (arr[x][k] == 0) {       /* 处理格为空 */
                    arr[x][k] = arr[x][y];  // 滑至处理格
                    arr[x][y] = 0;          // 当前格置空
                    y += delta;             // 当前格指向下一格
                    moved = 1;
                } else {                    /* 处理格不为空 */
                    if (arr[x][y] == arr[x][k]) {   /* 可以合并 */
                        arr[x][k] += arr[x][y];     // 合并当前格
                        arr[x][y] = 0;              // 当前格置空
                        score += arr[x][k];         // 分数加上合并值
                        y += delta;                 // 当前格指向下一格
                        k += delta;                 // 处理格指向下一格
                        moved = 1;
                    } else {                        /* 不能合并 */
                        k += delta;                 // 处理格指向下一格
                        if (k == y)                 // 若处理格指向当前格
                            y += delta;             // 则当前格指向下一格
                    }
                }
            } else {                /* 当前格为空 */
                y += delta;         // 当前格指向下一格
            }
        }
    }
    return moved;
}

// /* 左右移动合并 */
// int move_left_right(int **arr, int n, int direction)
// {
//     int x, y, z;
//     int moved = 1, res = 0;
//     while (moved)
//     {
//         moved = 0;
//         for (y = 0; y < n; ++y)
//         {
//             for (x = n - 1; x > 0; --x)
//             {
//                 if (arr[x][y] != 0 && arr[x][y] == arr[x-1][y])
//                 {
//                     arr[x-1][y] += arr[x][y];
//                     score += arr[x][y];
//                     arr[x][y] = 0;
//                     moved = 1;
//                     res = 1;
//                 }
//                 if (direction == GAME_KEY_LEFT &&
//                     arr[x-1][y] == 0 && arr[x][y] != 0)
//                 {
//                     arr[x-1][y] = arr[x][y];
//                     arr[x][y] = 0;
//                     moved = 1;
//                     res = 1;
//                 }
//                 if (direction == GAME_KEY_RIGHT &&
//                     arr[x-1][y] != 0 && arr[x][y] == 0)
//                 {
//                     arr[x][y] = arr[x-1][y];
//                     arr[x-1][y] = 0;
//                     moved = 1;
//                     res = 1;
//                 }
//             }
//         }
//     }
//     return res;
// }

// /* 上下移动合并 */
// int move_up_down(int **arr, int n, int direction)
// {
//     int x, y, z;
//     int moved = 1, res = 0;
//     while (moved)
//     {
//         moved = 0;
//         for (x = 0; x < n; ++x)
//         {
//             for (y = n - 1; y > 0; --y)
//             {
//                 if (arr[x][y] != 0 && arr[x][y] == arr[x][y-1])
//                 {
//                     arr[x][y-1] += arr[x][y];
//                     score += arr[x][y];
//                     arr[x][y] = 0;
//                     moved = 1;
//                     res = 1;
//                 }
//                 if (direction == GAME_KEY_UP &&
//                     arr[x][y-1] == 0 && arr[x][y] != 0)
//                 {
//                     arr[x][y-1] = arr[x][y];
//                     arr[x][y] = 0;
//                     moved = 1;
//                     res = 1;
//                 }
//                 if (direction == GAME_KEY_DOWN &&
//                     arr[x][y-1] != 0 && arr[x][y] == 0)
//                 {
//                     arr[x][y] = arr[x][y-1];
//                     arr[x][y-1] = 0;
//                     moved = 1;
//                     res = 1;
//                 }
//             }
//         }
//     }
//     return res;
// }

/* 刷新游戏界面 */
void print2048(int **arr, int n)
{
    int i, j, deltax, deltay, fix;
    int startx, starty;

    starty = (LINES - n * HEIGHT) / 2;
    startx = (COLS - n * WIDTH) / 2;
    clear();
    board(stdscr, starty, startx, n, n, WIDTH, HEIGHT);
    deltay = HEIGHT / 2;
    deltax = WIDTH / 2;
    mvprintw(starty + n * HEIGHT - 2, startx + n * WIDTH + 1, GAME_TEXT_RESTART);
    mvprintw(starty + n * HEIGHT - 1, startx + n * WIDTH + 1, GAME_TEXT_EXIT);
    attron(A_BOLD);
    mvprintw(starty - 2, (COLS - sizeof(GAME_TEXT_TITLE)) / 2, GAME_TEXT_TITLE);
    mvprintw(starty, startx + n * WIDTH + 1, GAME_TEXT_SCORE, score);
    topscore = score > topscore ? score : topscore;
    if (topscore > 0)
        mvprintw(starty + 1, startx + n * WIDTH + 1, GAME_TEXT_TOPSCORE, topscore);
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            if (arr[i][j] != 0) {
                fix = arr[i][j] > 1000 ? 2 : arr[i][j] > 100 ? 1 : 0;
                mvprintw(starty + j * HEIGHT + deltay,
                    startx + i * WIDTH + deltax - fix,
                    "%d", arr[i][j]);
            }
    /* 游戏结束 */
    if (gameoverflag) {
        attron(A_BLINK);
        mvprintw(starty + n * HEIGHT + 1, (COLS - sizeof(GAME_TEXT_OVER)) / 2, GAME_TEXT_OVER);
        attroff(A_BLINK);
    }
    attroff(A_BOLD);
    refresh();
}
