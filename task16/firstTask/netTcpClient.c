#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_LENGTH_MSG 20
#define PORT_SERVER 7777
int main()
{
  struct sockaddr_in server;
  char *sendMsg = "Hi";
  char recvMsg[MAX_LENGTH_MSG];

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT_SERVER);
  server.sin_addr.s_addr = inet_addr("127.0.0.1");

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
    perror("Error recv");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", recvMsg);

  close(fd);

  return 0;
}
