#include <stdint.h>
#include <stdio.h>
void printBin(int number)
{
    if (number >= 0)
    {
        for (int i = sizeof(number) * 8; i > 0; i--)
        {
            printf("%d", (number >> (i - 1)) & 1);
        }
        printf("\n");
    }
}
void printNegativeBin(int number)
{
    if (number < 0)
    {
        for (int i = sizeof(number) * 8; i > 0; i--)
        {
            printf("%d", (number >> (i - 1)) & 1);
        }
        printf("\n");
    }
}
int countOnes(int number)
{
    int count = 0;
    for (int i = 0; i < sizeof(number) * 8; i++)
    {
        if (((number >> i) & 1) == 1)
        {
            count++;
        }
    }
    return count;
}
int replaceThirdByte(int number, uint8_t byte)
{
    return ((((number >> 16) & 0) | byte) << 16) | number;
}
int main()
{
    int number = 0;
    uint8_t byte = 0;
    printf("Представление целого неотрицательного числа в двоичной форме\n");
    printf("Введите целое неотрицательно число\n");
    scanf("%d", &number);
    printBin(number);

    printf("Представление целого отрицательного числа в двоичной форме\n");
    printf("Введите целое отрицательно число\n");
    scanf("%d", &number);
    printNegativeBin(number);

    printf("Подсчет количества единиц в двоичном представлении целого числа\n");
    printf("Введите любое целое число\n");
    scanf("%d", &number);
    printf("Количество единиц в двоичном представлении: %d\n", countOnes(number));

    printf("Замена третьего байта\n");
    printf("Введите исходное число\n");
    scanf("%d", &number);
    printf("Введите число на которое поменяется третий байт\n");
    scanf("%hhu", &byte);
    printf("%d\n", replaceThirdByte(number, byte));
    return 0;
}