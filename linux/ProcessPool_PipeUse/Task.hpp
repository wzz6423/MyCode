#pragma once

// cpp include
#include <iostream>
#include <string>
#include <vector>
#include <functional>
// c of cpp include
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cassert> // Makefile debug模式编译 要带-g
// system call include
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// for test
// 函数指针实现
typedef void (*task_t)();

// functional实现
// using task_t = std::function<void()>;

void task1(){
    std::cout << "Task1 ...aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
}

void task2(){
    std::cout << "Task2 ...aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
}

void task3(){
    std::cout << "Task3 ...aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
}

void task4(){
    std::cout << "Task4 ...aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
}

void LoadTask(std::vector<task_t> *tasks){
    tasks->push_back(task1);
    tasks->push_back(task2);
    tasks->push_back(task3);
    tasks->push_back(task4);
}