#include "game.h"

time_t startTime;

//��ӭ����
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

//������Ϸ����
void set_argument() {
    printf("��������Ϸ������\n");
    printf("\033[33m����\033[0m (9-20): ");
    scanf("%d", &ROW);
    while (ROW < 9 || ROW > 20) {
        printf("\033[33m����\033[0mӦ��9��20֮�䣬���������룺");
        scanf("%d", &ROW);
    }

    printf("\033[32m����\033[0m (9-20): ");
    scanf("%d", &LINE);
    while (LINE < 9 || LINE > 20) {
        printf("\033[32m����\033[0mӦ��9��20֮�䣬���������룺");
        while (getchar() != '\n');
        scanf("%d", &LINE);
    }

    printf("�׵����� (10-%d): ", (ROW * LINE) / 10 + 2);
    scanf("%d", &MINE);
    while (MINE < 10 || MINE >(ROW * LINE) / 10 + 2) {
        printf("�׵�����Ӧ��10��%d֮�䣬���������룺", (ROW * LINE) / 10 + 2);
        while (getchar() != '\n');
        scanf("%d", &MINE);
    }
}

//������Ϸ�˵�
void menu() {
    int input = 0;
    startTime = time(NULL); // ��¼��Ϸ��ʼʱ��
    srand((unsigned int)startTime);

    do {
        printf("��ѡ��\n");
        printf("0. �˳���Ϸ\n");
        printf("1. ������Ϸ\n");
        if (scanf("%d", &input) != 1) {
            while (getchar() != '\n');
            printf("������Ч��������һ�����֣�\n");
            continue;
        }
        switch (input) {
        case 0:
            printf("��ӭ���´�����\n");
            break;
        case 1:
            printf("��ӭ������Ϸ\n");
            set_argument();
            game();
            break;
        default:
            printf("������Ч�����������룺\n");
            break;
        }
    } while (input);
}

//��Ϸ�˵�
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

    printf("��ʼɨ����Ϸ\n");

    findMines(show, mine, LINE + 2, ROW + 2);

    for (int i = 0; i <= LINE + 2; i++) {
        free(show[i]);
        free(mine[i]);
    }
}

//��ӡ����
void printBoard(char** arr, int line, int row) {
    time_t currentTime = time(NULL);
    int elapsedTime = (int)(currentTime - startTime); // ���㾭����ʱ�䣨�룩

    printf("| �׵�������%d |", MINE);
    if (MINE <= 15) {
        printf(" ���Ѷ� |\n");
    }
    else if ((MINE > 15) && (MINE <= 30)) {
        printf(" �е��Ѷ� |\n");
    }
    else {
        printf(" �����Ѷ� |\n");
    }
    printf("| ����ʱ�䣺%d �� |\n", elapsedTime); // ��ʾ������ʱ��

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

//��ʼ�����
void initializeBoard(char** arr, char con) {
    for (int i = 0; i < LINE + 2; i++) {
        for (int j = 0; j < ROW + 2; j++) {
            arr[i][j] = (i == 0 || i == LINE + 1 || j == 0 || j == ROW + 1) ? ' ' : con;
        }
    }
}

//������
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

//ͳ��ĳ��������Χ�׵�����
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

//����
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

//���
void mark(char** show, int line, int row) {
    int x, y;
    char input[10];
    while (true) {
        printf("����������Ҫ���б�ǵ�����(��ʽ: \033[33mx\033[0m \033[32my\033[0m)��\n");
        if (scanf("%d%d", &x, &y) != 2 || x < 1 || x > line || y < 1 || y > row || show[x][y] != '*') {
            while (getchar() != '\n');
            printf("�����������Ч���ѱ��Ų飬���������룺\n");
            continue;
        }
        show[x][y] = '#';
        printBoard(show, line - 2, row - 2);
        printf("����Ҫ�������б���𣿣�0.����Ҫ 1.��Ҫ 2.ȡ����ǣ���\n");
        scanf("%s", input);
        if (strcmp(input, "0") == 0) {
            break;
        }
        else if (strcmp(input, "1") == 0) {
            continue;
        }
        else if (strcmp(input, "2") == 0) {
            printf("����������Ҫȡ����ǵ�����(��ʽ: \033[33mx\033[0m \033[32my\033[0m)��\n");
            if (scanf("%d%d", &x, &y) != 2 || x < 1 || x > line || y < 1 || y > row || show[x][y] != '#') {
                while (getchar() != '\n');
                printf("�����������Ч��δ����ǣ����������룺\n");
                continue;
            }
            show[x][y] = '*';
            printBoard(show, line - 2, row - 2);
            printf("����Ҫ�������б���𣿣�0.����Ҫ 1.��Ҫ����\n");
            scanf("%s", input);
            if (strcmp(input, "0") == 0) {
                break;
            }
            else if (strcmp(input, "1") == 0) {
                continue;
            }
            else {
                printf("������Ч��������0��1��2��\n");
            }
        }
        else {
            printf("������Ч��������0��1��2��\n");
        }
    }
}

//����
void findMines(char** show, char** mine, int line, int row) {
    int x, y;
    bool visited[20][20] = { false };

    while (true) {
        printBoard(show, line - 2, row - 2);
        int choice = 0;
        printf("���Ƿ���Ҫ���б�ǣ�\n");
        printf("0.����Ҫ����������   1.��Ҫ\n");
        printf("����������ѡ��\n");
        scanf("%d", &choice);
        if (choice) {
            mark(show, line, row);
        }
        printf("������Ҫ�Ų������ (��ʽ: \033[33mx\033[0m \033[32my\033[0m): ");
        if (scanf("%d %d", &x, &y) != 2 || x < 1 || x > line || y < 1 || y > row || show[x][y] != '*') {
            while (getchar() != '\n');
            printf("�����������Ч���ѱ��Ų飬���������룺\n");
            continue;
        }

        if (mine[x][y] == '1') {
            printf("���ź����㱻ը����\n");
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
            printf("��ϲ�㣬���׳ɹ�\n");
            printBoard(mine, line - 2, row - 2);
            break;
        }
    }
}