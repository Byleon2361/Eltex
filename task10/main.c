#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  int ppid =0;
  int pid =0;
  pid = fork();
  if(pid == 0)
  {
    pid = getpid();
    ppid = getppid();
    printf("I am child, my pid=%d, my ppid=%d\n", pid,ppid);
  }
  else if(pid > 0)
  {
    pid = getpid();
    ppid = getppid();
    printf("I am parent, my pid=%d, my ppid=%d\n", pid,ppid);
  }
  wait(NULL);
}
