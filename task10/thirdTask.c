#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH_NAME_PROGRAM 32
#define MAX_LENGTH_FULL_PATH 48
#define MAX_LENGTH_OPTIONS 32

int main()
{
  char progName[MAX_LENGTH_NAME_PROGRAM];
  char progOpt[MAX_LENGTH_OPTIONS];
  char fullPathToApp[MAX_LENGTH_FULL_PATH];
  pid_t pid;
  
  while(1)
  {
    printf("Enter the program name(enter \"exit\" to exit)\n");
    scanf("%31s", progName);
    printf("%s\n", progName);
    if(strcmp(progName, "exit") == 0)
    {
      break;
    }

    printf("Enter the start options\n");
    scanf("%31s", progOpt);

    strncpy(fullPathToApp, "/usr/bin/", sizeof(fullPathToApp));
    strcat(fullPathToApp, progName);

    pid = fork();
    if(pid == 0)
    {
      printf("I am child, my pid=%d, my ppid=%d\n", getpid(), getppid());
    printf("%s\n", fullPathToApp);
    printf("%s\n", progName);
    printf("%s\n", progOpt);
      if(execl(fullPathToApp,progName, progOpt, (char*)NULL)<0)
      {
        perror("Failed to execute program");
      }
      exit(0);
    }
    wait(NULL);
  }
  return 0;
}

