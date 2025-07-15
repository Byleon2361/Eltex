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
  char *sendMsg = "Hi";
  char recvMsg[MAX_LENGTH_MSG];

  int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  client.sun_family = AF_LOCAL;
  strncpy(client.sun_path, SOCK_PATH_CLIENT, sizeof(client.sun_path));

  if(bind(fd, (struct sockaddr *)&client, sizeof(struct sockaddr_un)) == -1)
  {
    close(fd);
    perror("Error bind");
    exit(EXIT_FAILURE);
  }

  server.sun_family = AF_LOCAL;
  strncpy(server.sun_path,SOCK_PATH_SERVER, sizeof(server.sun_path));
  if(connect(fd, (struct sockaddr*)&server, sizeof(server)) == -1)
  {
    close(fd);
    perror("Error connect");
    exit(EXIT_FAILURE);
  }

  send(fd, sendMsg, strlen(sendMsg)+1, 0);
  recv(fd, recvMsg, MAX_LENGTH_MSG, 0);

  printf("%s\n", recvMsg);

  close(fd);

  return 0;
}
