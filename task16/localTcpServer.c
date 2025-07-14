#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#define SOCK_PATH "localTcpPath"
#define MAX_LENGTH_QUEUE_CLIENTS 1
#define MAX_LENGTH_MSG 20
int main()
{
  struct sockaddr_un server, client;
  char *sendMsg = "Hello";
  char recvMsg[MAX_LENGTH_MSG];

  int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  server.sun_family = AF_LOCAL;
  strncpy(server.sun_path, SOCK_PATH, sizeof(server.sun_path));

  if(bind(fd, (struct sockaddr *)&server, sizeof(struct sockaddr_un)) == -1)
  {
    close(fd);
    unlink(SOCK_PATH);
    perror("Error bind");
    exit(EXIT_FAILURE);
  }

  listen(fd, MAX_LENGTH_QUEUE_CLIENTS);

  socklen_t sockClientLen;
  int newFd = accept(fd, (struct sockaddr*)&client, &sockClientLen);
  if(newFd == -1)
  {
    close(fd);
    unlink(SOCK_PATH);
    perror("Error create new socket");
    exit(EXIT_FAILURE);
  }

  send(newFd, sendMsg, strlen(sendMsg)+1, 0);
  recv(newFd, recvMsg, MAX_LENGTH_MSG, 0);

  printf("%s\n", recvMsg);

  close(newFd);
  close(fd);
  unlink(SOCK_PATH);

  return 0;
}
