#include "src/libcalc.h"
#include <stdio.h>

void inputNumbers(int* first, int* second)
{
    printf("Введите первое число\n");
    scanf("%d", first);
    printf("Введите второе число\n");
    scanf("%d", second);
}
int main()
{
    int number = 0;

    int first = 0;
    int second = 0;

    while (number != 5)
    {

        printf("1)Сложени\n2)Вычитание\n3)Умножение\n4)Деление\n5)Выход\n");
        scanf("%d", &number);
        switch (number)
        {
            case 1:

                inputNumbers(&first, &second);
                printf("Результат сложения: %d\n", myAdd(first, second));
                break;
            case 2:
                inputNumbers(&first, &second);
                printf("Результат вычитания: %d\n", mySub(first, second));
                break;
            case 3:
                inputNumbers(&first, &second);
                printf("Результат умножения: %d\n", myMul(first, second));
                break;
            case 4:
                inputNumbers(&first, &second);
                printf("Результат деления: %d\n", myDiv(first, second));
                break;
            case 5: break;
            default: printf("Неверный номер"); break;
        }
    }
    return 0;
}