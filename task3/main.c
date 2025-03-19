#include <stdint.h>
#include <stdio.h>
#include <string.h>
// задание 1
int replaceThirdByte(int number, uint8_t byte)
{
    uint8_t* ptr = (uint8_t*)&number;
    *(ptr + 2) = byte;
    return number;
}
// задание 3
void printArray(int* arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d ", *(arr + i));
    }
    printf("\n");
}
// задание 4
char* sstr(char* str, char* underStr)
{
    int n = 1;
    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = 0; j < strlen(underStr); j++)
        {
            if (*(str + i + j) == *(underStr + j))
            {
                if (n == strlen(underStr))
                {
                    return underStr;
                }
                n++;
                continue;
            }
            break;
        }
    }

    return NULL;
}
int main()
{
    uint8_t byte = 0;
    int number = 0;
    // 1
    printf("Замена третьего байта\n");
    printf("Введите число\n");
    scanf("%d", &number);
    printf("Введите число, которое заменит собой третий байт\n");
    scanf("%hhu", &byte);
    printf("%d\n", replaceThirdByte(number, byte));

    // 3
    printf("Вывод массива от 1 до 10 с помощью указателя\n");
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    printArray(arr, sizeof(arr) / sizeof(arr[0]));

    // 4
    printf("Поиск подстроки в строке\n");
    char* str = "Hello, world";
    char* underStr = "world";
    char* newStr = sstr(str, underStr);
    if (newStr != NULL)
        printf("%s", newStr);
    else
        printf("Подстрока не найдена\n");
    return 0;
}