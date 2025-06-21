#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 256
#define COMMON_NAME_PIPE "pipe"

int main()
{
  int fd;
  int status;
  char sendBuf[MAX_BUFFER_SIZE] = "Hello, world";

  status = mkfifo(COMMON_NAME_PIPE, 0666);
  if(status == -1)
  {
    perror("Failed mkfifo");
    exit(EXIT_FAILURE);
  }

  fd = open(COMMON_NAME_PIPE, O_WRONLY);
  if(fd == -1)
  {
    perror("Failed open file");
    exit(EXIT_FAILURE);
  }
  write(fd, sendBuf, strlen(sendBuf));
  close(fd);
  return 0;
}
