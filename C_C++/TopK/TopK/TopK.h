#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

typedef int HPDataType;

void AdjustDown(HPDataType* a, int n, int parent);
void Swap(HPDataType* p1, HPDataType* p2);