#include "libcalc.h"
double myDiv(double a, double b)
{
    if (b == 0)
    {
        printf("Ошибка деления\n");
        exit(1);
    }
    return a / b;
}