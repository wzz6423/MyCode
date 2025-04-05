#pragma once

#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <string.h>

extern int ROW;
extern int LINE;
extern int MINE;
extern time_t startTime;

#define ROWS (ROW + 2)
#define LINES (LINE + 2)

void hello();
void menu();
void game();
void printBoard(char** arr, int line, int row);
void initializeBoard(char** arr, char con);
void setMines(char** arr, int line, int row);
void markSafePositions(char** show, char** mine, int x, int y, bool visited[20][20], int line, int row);
void mark(char** show, int line, int row);
void findMines(char** show, char** mine, int line, int row);
int getMineCount(char** mine, int x, int y);