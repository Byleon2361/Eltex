#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#define SOCK_PATH "localTcpPath"
#define MAX_LENGTH_MSG 20
int main()
{
  struct sockaddr_un server;
  char *sendMsg = "Hi";
  char recvMsg[MAX_LENGTH_MSG];

  int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  memset(&server, 0, sizeof(server));
  server.sun_family = AF_LOCAL;
  strncpy(server.sun_path, SOCK_PATH, sizeof(server.sun_path));

  if(connect(fd, (struct sockaddr*)&server, sizeof(server)) == -1)
  {
    close(fd);
    perror("Error connect");
    exit(EXIT_FAILURE);
  }

  if(send(fd, sendMsg, strlen(sendMsg)+1, 0) == -1)
  {
    close(fd);
    perror("Error send");
    exit(EXIT_FAILURE);
  }
  int bytes = recv(fd, recvMsg, MAX_LENGTH_MSG, 0);
  if(bytes <= 0)
  {
    close(fd);
    perror("Error send");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", recvMsg);

  close(fd);

  return 0;
}
