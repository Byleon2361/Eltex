#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 256
#define COMMON_NAME_PIPE "pipe"

int main()
{
  int fd = 0;
  char recieveBuf[MAX_BUFFER_SIZE] = {0};
  
  fd = open(COMMON_NAME_PIPE, O_RDONLY);
  if(fd == -1)
  {
    perror("Failed open file");
    exit(EXIT_FAILURE);
  }
  read(fd, recieveBuf, MAX_BUFFER_SIZE);
  close(fd);

  printf("%s\n", recieveBuf);

  return 0;
}
