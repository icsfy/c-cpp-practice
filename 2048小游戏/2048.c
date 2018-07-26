#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

/* 每一格的宽和高 */
#define WIDTH 6
#define HEIGHT 4
// #define WIDTH 4
// #define HEIGHT 2

/* 游戏中随机出现数字2和4, 定义数字2出现概率百分比 */
#define PERCENT_2   90

/* 游戏按键控制上下左右, 重新开始和退出 */
#define GAME_KEY_LEFT       'a'
#define GAME_KEY_RIGHT      'd'
#define GAME_KEY_UP         'w'
#define GAME_KEY_DOWN       's'
#define GAME_KEY_RESTART    'r'
#define GAME_KEY_EXIT       'q'

/* 游戏中的一些提示文字 */
#define GAME_TEXT_TITLE     " --- 2048 ---"
#define GAME_TEXT_SCORE     "score: %d"
#define GAME_TEXT_TOPSCORE  "Top: %d"
#define GAME_TEXT_NOTMOVE   "!cannot move this way!"
#define GAME_TEXT_OVER      " GAME OVER!"
#define GAME_TEXT_RESTART   "'r': Restart game"
#define GAME_TEXT_EXIT      "'q': Quit game"

/* 游戏结束标志 */
int gameoverflag;

/* 游戏分数 */
int score, topscore;

/* 输出方框边界 */
void board(WINDOW *win, int starty, int startx, int lines, int cols,
           int tile_width, int tile_height);

/* 检查是否能继续游戏 */
void check2048(int **arr, int n);

/* 合并数字 */
int moveleft(int **arr, int n);
int moveright(int **arr, int n);
int moveup(int **arr, int n);
int movedown(int **arr, int n);

/* 生成随机数字2或4 */
void update2048(int **arr, int n, int count);

/* 刷新游戏界面 */
void print2048(int **arr, int n);

int main(int argc, char const *argv[])
{
    int **arr, n = 4, ch, res;
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
                res = moveleft(arr, n);
                break;
            case KEY_RIGHT:
            case GAME_KEY_RIGHT:
                res = moveright(arr, n);
                break;
            case KEY_UP:
            case GAME_KEY_UP:
                res = moveup(arr, n);
                break;
            case KEY_DOWN:
            case GAME_KEY_DOWN:
                res = movedown(arr, n);
                break;
            case GAME_KEY_RESTART:
                goto FREE_RESOURCE;
                break;
        }
        if (res) {
            update2048(arr, n, 1);
            check2048(arr, n);
            print2048(arr, n);
        } else if (!gameoverflag) {
            attron(A_BLINK);
            mvprintw(1, 1, GAME_TEXT_NOTMOVE);
            attroff(A_BLINK);
        }
    }
FREE_RESOURCE:
    endwin();
    /* 释放资源 */
    for (int i = 0; i < n; ++i)
        free(arr[i]);
    free(arr);
    if (ch == 'r')
        goto LABEL_RESTART;
    return 0;
}

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

void update2048(int **arr, int n, int count)
{
    if (gameoverflag) return;
    int x, y, num, flag;
    while (count--)
    {
        flag = 0;
        for (x = 0; x < n && !flag; ++x)
            for (y = 0; y < n && !flag; ++y)
                if (arr[x][y] == 0)
                    flag = 1;
        num = rand() % 100 < PERCENT_2 ? 2 : 4;
        while (flag)
        {
            x = rand() % n;
            y = rand() % n;
            if (arr[x][y] == 0)
            {
                arr[x][y] = num;
                flag = 0;
            }
        }
    }
}

int moveleft(int **arr, int n)
{
    int x, y, z;
    int moved = 1, res = 0;
    while (moved)
    {
        moved = 0;
        for (y = 0; y < n; ++y)
        {
            for (x = n - 1; x > 0; --x)
            {
                if (arr[x][y] != 0 && arr[x][y] == arr[x-1][y])
                {
                    arr[x-1][y] += arr[x][y];
                    score += arr[x][y];
                    arr[x][y] = 0;
                    moved = 1;
                    res = 1;
                }
                if (arr[x-1][y] == 0 && arr[x][y] != 0)
                {
                    arr[x-1][y] = arr[x][y];
                    arr[x][y] = 0;
                    moved = 1;
                    res = 1;
                }
            }
        }
    }
    return res;
}

int moveright(int **arr, int n)
{
    int x, y, z;
    int moved = 1, res = 0;
    while (moved)
    {
        moved = 0;
        for (y = 0; y < n; ++y)
        {
            for (x = n - 2; x >= 0; --x)
            {
                if (arr[x][y] != 0 && arr[x][y] == arr[x+1][y])
                {
                    arr[x+1][y] += arr[x][y];
                    score += arr[x][y];
                    arr[x][y] = 0;
                    moved = 1;
                    res = 1;
                }
                if (arr[x+1][y] == 0 && arr[x][y] != 0)
                {
                    arr[x+1][y] = arr[x][y];
                    arr[x][y] = 0;
                    moved = 1;
                    res = 1;
                }
            }
        }
    }
    return res;
}

int moveup(int **arr, int n)
{
    int x, y, z;
    int moved = 1, res = 0;
    while (moved)
    {
        moved = 0;
        for (x = 0; x < n; ++x)
        {
            for (y = n - 1; y > 0; --y)
            {
                if (arr[x][y] != 0 && arr[x][y] == arr[x][y-1])
                {
                    arr[x][y-1] += arr[x][y];
                    score += arr[x][y];
                    arr[x][y] = 0;
                    moved = 1;
                    res = 1;
                }
                if (arr[x][y-1] == 0 && arr[x][y] != 0)
                {
                    arr[x][y-1] = arr[x][y];
                    arr[x][y] = 0;
                    moved = 1;
                    res = 1;
                }
            }
        }
    }
    return res;
}

int movedown(int **arr, int n)
{
    int x, y, z;
    int moved = 1, res = 0;;
    while (moved)
    {
        moved = 0;
        for (x = 0; x < n; ++x)
        {
            for (y = n - 2; y >= 0; --y)
            {
                if (arr[x][y] != 0 && arr[x][y] == arr[x][y+1])
                {
                    arr[x][y+1] += arr[x][y];
                    score += arr[x][y];
                    arr[x][y] = 0;
                    moved = 1;
                    res = 1;
                }
                if (arr[x][y+1] == 0 && arr[x][y] != 0)
                {
                    arr[x][y+1] = arr[x][y];
                    arr[x][y] = 0;
                    moved = 1;
                    res = 1;
                }
            }
        }
    }
    return res;
}

void print2048(int **arr, int n)
{
    int i, j, deltax, deltay;
    int startx, starty;
    int len;
    char strscore[sizeof(GAME_TEXT_SCORE)+10];

    starty = (LINES - n * HEIGHT) / 2;
    startx = (COLS - n * WIDTH) / 2;
    clear();
    board(stdscr, starty, startx, n, n, WIDTH, HEIGHT);
    deltay = HEIGHT / 2;
    deltax = WIDTH / 2;
    mvprintw(LINES - 2, startx, GAME_TEXT_RESTART);
    mvprintw(LINES - 1, startx, GAME_TEXT_EXIT);
    attron(A_BOLD);
    mvprintw(starty - 2, (COLS - sizeof(GAME_TEXT_TITLE)) / 2, GAME_TEXT_TITLE);
    len = sprintf(strscore, GAME_TEXT_SCORE, score);
    mvprintw(starty - 1, COLS - startx - len, strscore);
    topscore = score > topscore ? score : topscore;
    if (topscore > 0)
        mvprintw(starty - 1, startx, GAME_TEXT_TOPSCORE, topscore);
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            if (arr[i][j] != 0)
                mvprintw(starty + j * HEIGHT + deltay,
                    startx + i * WIDTH + deltax,
                    "%d", arr[i][j]);
    if (gameoverflag) {
        attron(A_BLINK);
        mvprintw(starty + n * HEIGHT + 1, (COLS - sizeof(GAME_TEXT_OVER)) / 2, GAME_TEXT_OVER);
        attroff(A_BLINK);
    }
    attroff(A_BOLD);
    refresh();
}
