#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_BUFFER_SIZE 256

int main()
{
  int fd[2];
  int status;
  pid_t pid;
  char sendBuf[MAX_BUFFER_SIZE] = "Hello, world";
  char recieveBuf[MAX_BUFFER_SIZE];

  status = pipe(fd);
  if(status == -1)
  {
    perror("Failed pipe");
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if(pid == 0)
  {
    close(fd[1]);
    read(fd[0], recieveBuf, MAX_BUFFER_SIZE);
    close(fd[0]);
    printf("%s\n", recieveBuf);
    exit(EXIT_SUCCESS);
  }
  else if(pid > 0)
  {
    close(fd[0]);
    write(fd[1], sendBuf, MAX_BUFFER_SIZE);
    close(fd[1]);
    wait(NULL);
  }
  return 0;
}
