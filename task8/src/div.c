#include "libcalc.h"
int myDiv(int a, int b)
{
    if (b == 0)
    {
        printf("Ошибка деления\n");
        exit(1);
    }
    return a / b;
}