#include <stdio.h>
#include <string.h>
#define MAXSIZE 128
int main()
{
    FILE* file;
    file = fopen("output.txt", "w");
    if (file == NULL)
    {
        printf("Не удалось открыть файл\n");
        return 1;
    }

    char* str = "String from file";
    fwrite(str, sizeof(char), strlen(str), file);

    fclose(file);

    file = fopen("output.txt", "r");
    if (file == NULL)
    {
        printf("Не удалось открыть файл\n");
        return 1;
    }

    char res[MAXSIZE];
    fseek(file, 0, SEEK_END);
    int countSym = ftell(file) - 1;
    if (countSym < 0)
    {
        printf("Файл пуст\n");
    }

    int i;
    for (i = 0; fseek(file, countSym - i, SEEK_SET) == 0; i++)
    {
        if (i >= MAXSIZE) break;

        res[i] = fgetc(file);
    }
    res[i] = '\0';

    printf("%s\n", res);

    fclose(file);
    return 0;
}