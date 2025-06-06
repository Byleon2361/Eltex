#include <linux/limits.h>
#include <stdio.h>
#include <dirent.h>
#include <regex.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#define REG_EX "^lib.*\\.so$"
#define MAX_FUNC_NAME 64
int mySelector(const struct dirent *dirent)
{
  char* regExStr = REG_EX;
  regex_t regEx;

  if(regcomp(&regEx, regExStr, REG_EXTENDED)!=0)
  {
    perror("Не удалось скомпилировать регулярное выражение\n");
    return 0;
  }
  int isRegular = regexec(&regEx, dirent->d_name, 0, NULL, 0);

  regfree(&regEx);
  return (isRegular == 0);
}
char* fetchFuncName(char *funcName, const char* libName)
{
  char *temp;
  strcpy(funcName, libName+3);

  temp = strchr(funcName, '.');
  *temp = '\0';

  return funcName;
}
int main()
{
  struct dirent** dirent = NULL;
  char pathLib[PATH_MAX];
  char funcName[MAX_FUNC_NAME];
  char* error;

  int n = scandir(".", &dirent, mySelector, 0);
  int (*funcPtr)(int,int);

  void* dlHandle; 
  for(int i = 0 ; i < n; i++)
  {
    snprintf(pathLib, sizeof(pathLib), "./%s", dirent[i]->d_name);
    fetchFuncName(funcName,dirent[i]->d_name);

    printf("%s\n",pathLib);
    printf("%s\n", dirent[i]->d_name);
    printf("%s\n",funcName);

    dlHandle = dlopen(pathLib, RTLD_LAZY);
    if(dlHandle == NULL)
    {
      error = dlerror();
      fprintf(stderr, "Не удалось открыть библиотеку - %s\n", error);
      return 1;
    }

    funcPtr =  dlsym(dlHandle, funcName);
    if(funcPtr == NULL)
    {
      error = dlerror();
      fprintf(stderr, "Не удалось открыть библиотеку - %s\n", error);
      return 1;
    }

    printf("%s: %d\n",funcName, funcPtr(2,2));

    if(dlclose(dlHandle) != 0 )
    {
      error = dlerror();
      fprintf(stderr, "Не удалось открыть библиотеку - %s\n", error);
      return 1;
    }
  }
  for(int i = 0 ; i < n; i++)
  {
    free(dirent[i]);
  }
  free(dirent);
  return 0;
}
