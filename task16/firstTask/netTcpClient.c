#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_LENGTH_MSG 20
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

  server.sin_family = AF_INET;
  server.sin_port = htons(8080);
  server.sin_addr.s_addr = inet_addr("127.0.0.1");

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
