#include <stdio.h>
#include <string.h>
#define MAXABONENTS 100
typedef struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
} Abonent;

Abonent abonents[MAXABONENTS];
int count;

void printAbonent(Abonent abonent)
{
    printf("Имя: %s\n", abonent.name);
    printf("Фамилия: %s\n", abonent.second_name);
    printf("Номер: %s\n", abonent.tel);
    printf("--------------------------\n");
}
void printAllAbonents()
{
    for (int i = 0; i < count; i++)
    {
        printAbonent(abonents[i]);
    }
}
void addAbonent()
{

    if (count + 1 > MAXABONENTS)
    {
        printf("Структура переполнена\n");
        return;
    }
    for (int i = 0; i < MAXABONENTS; i++)
    {
        if (abonents[i].name[0] == '\000')
        {
            printf("Введите имя\n");
            scanf("%9s", abonents[i].name);
            printf("Введите фамилию\n");
            scanf("%9s", abonents[i].second_name);
            printf("Введите номер\n");
            scanf("%9s", abonents[i].tel);
            count++;
            break;
        }
    }
}
void removeAbonent()
{
    char name[10];
    printf("Введите имя абонента для удаления\n");
    scanf("%9s", name);
    for (int i = 0; i < count; i++)
    {
        if (strcmp(abonents[i].name, name) == 0)
        {
            count--;
            strcpy(abonents[i].name, abonents[count].name);
            strcpy(abonents[i].second_name, abonents[count].second_name);
            strcpy(abonents[i].tel, abonents[count].tel);
            memset(abonents + count, 0, sizeof(abonents[0]));
        }
    }
}
void searhAbonent()
{
    char name[10];
    printf("Введите имя для поиска\n");
    scanf("%9s", name);
    for (int i = 0; i < count; i++)
    {
        if (strcmp(abonents[i].name, name) == 0)
        {
            printAbonent(abonents[i]);
        }
    }
}

int main()
{
    memset(abonents, 0, MAXABONENTS * sizeof(abonents[0]));
    count = 0;

    int number = 0;
    while (number != 5)
    {

        printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по имени\n4) Вывод всех записей\n5) Выход\n");
        scanf("%d", &number);
        switch (number)
        {
            case 1: addAbonent(); break;
            case 2: removeAbonent(); break;
            case 3: searhAbonent(); break;
            case 4: printAllAbonents(); break;
            default: printf("Неверный номер"); break;
        }
    }
    return 0;
}