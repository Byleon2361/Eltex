#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
  pid_t pid =0;
  int status;

  pid = fork();
  if(pid == 0)
  {
    printf("I am child, my pid=%d, my ppid=%d\n", getpid(),getppid());
    exit(5);
  }
  else if(pid > 0)
  {
    printf("I am parent, my pid=%d, my ppid=%d\n", getpid(),getppid());
    wait(&status);
    printf("status=%d\n", WEXITSTATUS(status));
  }
}
