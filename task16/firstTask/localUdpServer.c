#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#define SOCK_PATH_SERVER "localUdpPathServer"
#define SOCK_PATH_CLIENT "localUdpPathClient"
#define MAX_LENGTH_MSG 20
int main()
{
  struct sockaddr_un server, client;
  char *sendMsg = "Hello";
  char recvMsg[MAX_LENGTH_MSG];

  int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  memset(&server, 0, sizeof(server));
  server.sun_family = AF_LOCAL;
  strncpy(server.sun_path, SOCK_PATH_SERVER, sizeof(server.sun_path));

  if(bind(fd, (struct sockaddr *)&server, sizeof(struct sockaddr_un)) == -1)
  {
    close(fd);
    unlink(SOCK_PATH_SERVER);
    unlink(SOCK_PATH_CLIENT);
    perror("Error bind");
    exit(EXIT_FAILURE);
  }

  socklen_t clientLen = sizeof(client);
  int bytes = recvfrom(fd, recvMsg, MAX_LENGTH_MSG, 0, (struct sockaddr *)&client, &clientLen);
  if(bytes <= 0)
  {
    close(fd);
    unlink(SOCK_PATH_SERVER);
    unlink(SOCK_PATH_CLIENT);
    perror("Error recv");
    exit(EXIT_FAILURE);
  }
  if(sendto(fd, sendMsg, strlen(sendMsg)+1, 0, (struct sockaddr *)&client, clientLen) == -1)
  {
    close(fd);
    unlink(SOCK_PATH_SERVER);
    unlink(SOCK_PATH_CLIENT);
    perror("Error send");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", recvMsg);

  close(fd);
  unlink(SOCK_PATH_SERVER);
  unlink(SOCK_PATH_CLIENT);

  return 0;
}
