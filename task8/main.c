#include <linux/limits.h>
#include <stdio.h>
#include <dirent.h>
#include <regex.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#define REG_EX "^lib.*\\.so$"
#define MAX_FUNC_NAME 64
#define MAX_MENU 256
#define MAX_COUNT_FUNC 16
int mySelector(const struct dirent* dirent)
{
    char* regExStr = REG_EX;
    regex_t regEx;

    if (regcomp(&regEx, regExStr, REG_EXTENDED) != 0)
    {
        perror("Не удалось скомпилировать регулярное выражение\n");
        return 0;
    }
    int isRegular = regexec(&regEx, dirent->d_name, 0, NULL, 0);

    regfree(&regEx);
    return (isRegular == 0);
}
char* fetchFuncName(char* funcName, const char* libName)
{
    char* temp;
    strcpy(funcName, libName + 3);

    temp = strchr(funcName, '.');
    *temp = '\0';

    return funcName;
}
int main()
{
    struct dirent** dirent = NULL;
    char pathLib[PATH_MAX];
    char funcName[MAX_FUNC_NAME];
    char menu[MAX_MENU] = {0};
    char numStr[16];
    char* error;
    int a = 0;
    int b = 0;

    int n = scandir(".", &dirent, mySelector, 0);
    int (*funcPtr[MAX_COUNT_FUNC])(int, int);

    void* dlHandle[MAX_COUNT_FUNC];
    int i;
    for (i = 0; i < n; i++)
    {
        snprintf(pathLib, sizeof(pathLib), "./%s", dirent[i]->d_name);
        fetchFuncName(funcName, dirent[i]->d_name);

        dlHandle[i] = dlopen(pathLib, RTLD_LAZY);
        if (dlHandle[i] == NULL)
        {
            error = dlerror();
            fprintf(stderr, "Не удалось открыть библиотеку - %s\n", error);
            return 1;
        }

        funcPtr[i] = dlsym(dlHandle[i], funcName);
        if (funcPtr[i] == NULL)
        {
            error = dlerror();
            fprintf(stderr, "Не удалось открыть библиотеку - %s\n", error);
            return 1;
        }

        snprintf(numStr, sizeof(numStr), "%d)", i + 1);
        strcat(menu, numStr);
        strcat(menu, " Выполинть функцию ");
        strcat(menu, funcName);
        strcat(menu, "\n");
    }
    snprintf(numStr, sizeof(numStr), "%d)", i + 1);
    strcat(menu, numStr);
    strcat(menu, " Выход");
    strcat(menu, "\n");

    int choice = 0;
    while (choice - 1 != i)
    {
        printf("Введите нужный пункт меню\n%s\n", menu);
        scanf("%d", &choice);
        if (choice - 1 == i) break;
        printf("Введите первое число\n");
        scanf("%d", &a);
        printf("Введите второе число\n");
        scanf("%d", &b);
        if (choice >= 1 && choice <= i) printf("Результат: %d\n", funcPtr[choice - 1](a, b));
    }

    for (int j = 0; j < n; j++)
    {
        if (dlclose(dlHandle[j]) != 0)
        {
            error = dlerror();
            fprintf(stderr, "Не удалось открыть библиотеку - %s\n", error);
            return 1;
        }
    }
    for (int j = 0; j < n; j++)
    {
        free(dirent[j]);
    }
    free(dirent);
    return 0;
}
