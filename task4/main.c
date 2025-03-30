#include <stdio.h>
#include <string.h>
#define MAXABONENTS 100
typedef struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
} Abonent;
int main()
{
    Abonent abonents[MAXABONENTS];
    memset(abonents, 0, MAXABONENTS * sizeof(abonents[0]));
    int number = 0;
    int count = 0;
    char name[10];
    while (number != 5)
    {

        printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по имени\n4) Вывод всех записей\n5) Выход\n");
        scanf("%d", &number);
        switch (number)
        {
            case 1:

                if (count + 1 > MAXABONENTS)
                {
                    printf("Структура переполнена\n");
                    break;
                }
                for (int i = 0; i < MAXABONENTS; i++)
                {
                    if (abonents[i].name[0] == '\000')
                    {
                        printf("Введите имя\n");
                        scanf("%s", abonents[i].name);
                        printf("Введите фамилию\n");
                        scanf("%s", abonents[i].second_name);
                        printf("Введите номер\n");
                        scanf("%s", abonents[i].tel);
                        count++;
                        break;
                    }
                }
                break;
            case 2:

                printf("Введите имя абонента для удаления\n");
                scanf("%s", name);
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

                break;
            case 3:

                printf("Введите имя для поиска\n");
                scanf("%s", name);
                for (int i = 0; i < count; i++)
                {
                    if (strcmp(abonents[i].name, name) == 0)
                    {
                        printf("Имя: %s\n", abonents[i].name);
                        printf("Фамилия: %s\n", abonents[i].second_name);
                        printf("Номер: %s\n", abonents[i].tel);
                    }
                }

                break;
            case 4:

                for (int i = 0; i < count; i++)
                {

                    printf("Имя: %s\n", abonents[i].name);
                    printf("Фамилия: %s\n", abonents[i].second_name);
                    printf("Номер: %s\n", abonents[i].tel);
                    printf("--------------------------\n");
                }
                break;
        }
    }
    return 0;
}