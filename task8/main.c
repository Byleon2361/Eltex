#include <stdio.h>
#include <dirent.h>
#include <regex.h>
#include <stdlib.h>
#define REG_EX ".so$"
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
int main()
{
  struct dirent** dirent = NULL;
  int n = 0;
  n = scandir(".", &dirent, mySelector, 0);

  for(int i = 0 ; i < n; i++)
  {
    printf("%s\n", dirent[i]->d_name);
  }
  for(int i = 0 ; i < n; i++)
  {
    free(dirent[i]);
  }
  free(dirent);
  return 0;
}
