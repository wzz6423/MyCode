#include "game.h"

time_t startTime;

//欢迎界面
void hello() {
    char arr1[] = "\033[33;5mwelcome to the game!!!!!!!!!\033[0m";
    char arr2[100];
    int left = 0;
    size_t right = strlen(arr1) - 1;

    while (left <= right) {
        strncpy(arr2 + left, arr1 + left, 1);
        strncpy(arr2 + right, arr1 + right, 1);
        arr2[left + 1] = '\0';
        arr2[right + 1] = '\0';
        printf("%s\n", arr2);
        Sleep(100);
        system("cls");
        left++;
        right--;
    }
    printf("%s\n", arr1);
}

//设置游戏参数
void set_argument() {
    printf("请输入游戏参数：\n");
    printf("\033[33m行数\033[0m (9-20): ");
    scanf("%d", &ROW);
    while (ROW < 9 || ROW > 20) {
        printf("\033[33m行数\033[0m应在9到20之间，请重新输入：");
        scanf("%d", &ROW);
    }

    printf("\033[32m列数\033[0m (9-20): ");
    scanf("%d", &LINE);
    while (LINE < 9 || LINE > 20) {
        printf("\033[32m列数\033[0m应在9到20之间，请重新输入：");
        while (getchar() != '\n');
        scanf("%d", &LINE);
    }

    printf("雷的数量 (10-%d): ", (ROW * LINE) / 10 + 2);
    scanf("%d", &MINE);
    while (MINE < 10 || MINE >(ROW * LINE) / 10 + 2) {
        printf("雷的数量应在10到%d之间，请重新输入：", (ROW * LINE) / 10 + 2);
        while (getchar() != '\n');
        scanf("%d", &MINE);
    }
}

//进入游戏菜单
void menu() {
    int input = 0;
    startTime = time(NULL); // 记录游戏开始时间
    srand((unsigned int)startTime);

    do {
        printf("请选择：\n");
        printf("0. 退出游戏\n");
        printf("1. 进入游戏\n");
        if (scanf("%d", &input) != 1) {
            while (getchar() != '\n');
            printf("输入无效，请输入一个数字：\n");
            continue;
        }
        switch (input) {
        case 0:
            printf("欢迎您下次来玩\n");
            break;
        case 1:
            printf("欢迎您玩游戏\n");
            set_argument();
            game();
            break;
        default:
            printf("输入无效，请重新输入：\n");
            break;
        }
    } while (input);
}

//游戏菜单
void game() {
    char** show = (char**)malloc((LINE + 2) * sizeof(char*));
    char** mine = (char**)malloc((LINE + 2) * sizeof(char*));
    bool visited[20][20] = { false };

    for (int i = 0; i <= LINE + 2; i++) {
        show[i] = (char*)malloc((ROW + 2) * sizeof(char));
        mine[i] = (char*)malloc((ROW + 2) * sizeof(char));
    }

    initializeBoard(show, '*');
    initializeBoard(mine, '-');

    setMines(mine, LINE + 2, ROW + 2);

    printf("开始扫雷游戏\n");

    findMines(show, mine, LINE + 2, ROW + 2);

    for (int i = 0; i <= LINE + 2; i++) {
        free(show[i]);
        free(mine[i]);
    }
}

//打印棋盘
void printBoard(char** arr, int line, int row) {
    time_t currentTime = time(NULL);
    int elapsedTime = (int)(currentTime - startTime); // 计算经过的时间（秒）

    printf("| 雷的数量：%d |", MINE);
    if (MINE <= 15) {
        printf(" 简单难度 |\n");
    }
    else if ((MINE > 15) && (MINE <= 30)) {
        printf(" 中等难度 |\n");
    }
    else {
        printf(" 困难难度 |\n");
    }
    printf("| 已用时间：%d 秒 |\n", elapsedTime); // 显示经过的时间

    for (int i = 0; i <= line; i++) {
        if (i == 0) {
            for (int j = 0; j <= row; j++) {
                if (0 == j) {
                    printf(" %d", j);
                }
                else {
                    printf("\033[32m %d\033[0m", j);
                }
            }
            printf("\n");
        }
        else {
            if (i > 9) {
                printf("\033[33m%d \033[0m", i);

            }
            else {
                printf("\033[33m %d \033[0m", i);

            }
            for (int j = 1; j <= row; j++) {
                if (arr[i][j] == '#') {
                    if (j >= 9) {
                        printf("\033[31m%c  \033[0m", arr[i][j]);
                    }
                    else {
                        printf("\033[31m%c \033[0m", arr[i][j]);
                    }
                }
                else if (arr[i][j] == '*') {
                    if (j >= 9) {
                        printf("\033[37m%c  \033[0m", arr[i][j]);
                    }
                    else {
                        printf("\033[37m%c \033[0m", arr[i][j]);
                    }
                }
                else {
                    if (j >= 9) {
                        printf("\033[34m%c  \033[0m", arr[i][j]);
                    }
                    else {
                        printf("\033[34m%c \033[0m", arr[i][j]);
                    }
                }
            }
            printf("\n");
        }
    }
}

//初始化面板
void initializeBoard(char** arr, char con) {
    for (int i = 0; i < LINE + 2; i++) {
        for (int j = 0; j < ROW + 2; j++) {
            arr[i][j] = (i == 0 || i == LINE + 1 || j == 0 || j == ROW + 1) ? ' ' : con;
        }
    }
}

//设置雷
void setMines(char** arr, int line, int row) {
    int count = MINE;
    int x, y;
    while (count) {
        x = rand() % (line - 2) + 1;
        y = rand() % (row - 2) + 1;
        if (arr[x][y] == '-') {
            arr[x][y] = '1';
            count--;
        }
    }
}

//统计某个坐标周围雷的数量
int getMineCount(char** mine, int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (mine[x + dx][y + dy] == '1') {
                count++;
            }
        }
    }
    return count;
}

//排雷
void markSafePositions(char** show, char** mine, int x, int y, bool visited[20][20], int line, int row) {
    if (x < 1 || x > line - 1 || y < 1 || y > row - 1 || visited[x][y] || mine[x][y] == '1') {
        return;
    }

    visited[x][y] = true;
    int mineCount = getMineCount(mine, x, y);
    if (mineCount == 0) {
        show[x][y] = '0';
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx != 0 || dy != 0) {
                    markSafePositions(show, mine, x + dx, y + dy, visited, line, row);
                }
            }
        }
    }
    else {
        show[x][y] = mineCount + '0';
    }
}

//标记
void mark(char** show, int line, int row) {
    int x, y;
    char input[10];
    while (true) {
        printf("请输入您想要进行标记的坐标(格式: \033[33mx\033[0m \033[32my\033[0m)：\n");
        if (scanf("%d%d", &x, &y) != 2 || x < 1 || x > line || y < 1 || y > row || show[x][y] != '*') {
            while (getchar() != '\n');
            printf("输入的坐标无效或已被排查，请重新输入：\n");
            continue;
        }
        show[x][y] = '#';
        printBoard(show, line - 2, row - 2);
        printf("您还要继续进行标记吗？（0.不需要 1.需要 2.取消标记）：\n");
        scanf("%s", input);
        if (strcmp(input, "0") == 0) {
            break;
        }
        else if (strcmp(input, "1") == 0) {
            continue;
        }
        else if (strcmp(input, "2") == 0) {
            printf("请输入您想要取消标记的坐标(格式: \033[33mx\033[0m \033[32my\033[0m)：\n");
            if (scanf("%d%d", &x, &y) != 2 || x < 1 || x > line || y < 1 || y > row || show[x][y] != '#') {
                while (getchar() != '\n');
                printf("输入的坐标无效或未被标记，请重新输入：\n");
                continue;
            }
            show[x][y] = '*';
            printBoard(show, line - 2, row - 2);
            printf("您还要继续进行标记吗？（0.不需要 1.需要）：\n");
            scanf("%s", input);
            if (strcmp(input, "0") == 0) {
                break;
            }
            else if (strcmp(input, "1") == 0) {
                continue;
            }
            else {
                printf("输入无效，请输入0或1或2：\n");
            }
        }
        else {
            printf("输入无效，请输入0或1或2：\n");
        }
    }
}

//查雷
void findMines(char** show, char** mine, int line, int row) {
    int x, y;
    bool visited[20][20] = { false };

    while (true) {
        printBoard(show, line - 2, row - 2);
        int choice = 0;
        printf("您是否需要进行标记？\n");
        printf("0.不需要，进行排雷   1.需要\n");
        printf("请输入您的选择：\n");
        scanf("%d", &choice);
        if (choice) {
            mark(show, line, row);
        }
        printf("请输入要排查的坐标 (格式: \033[33mx\033[0m \033[32my\033[0m): ");
        if (scanf("%d %d", &x, &y) != 2 || x < 1 || x > line || y < 1 || y > row || show[x][y] != '*') {
            while (getchar() != '\n');
            printf("输入的坐标无效或已被排查，请重新输入：\n");
            continue;
        }

        if (mine[x][y] == '1') {
            printf("很遗憾，你被炸死了\n");
            printBoard(mine, line - 2, row - 2);
            break;
        }

        markSafePositions(show, mine, x, y, visited, line, row);
        bool allSafeMarked = true;
        for (int i = 1; i < line; i++) {
            for (int j = 1; j < row; j++) {
                if (mine[i][j] != '1' && show[i][j] == '*') {
                    allSafeMarked = false;
                    break;
                }
            }
            if (!allSafeMarked) break;
        }

        if (allSafeMarked) {
            printf("恭喜你，排雷成功\n");
            printBoard(mine, line - 2, row - 2);
            break;
        }
    }
}