#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_LENGTH_QUEUE_CLIENTS 1
#define MAX_LENGTH_MSG 20
int main()
{
  struct sockaddr_in server, client;
  char *sendMsg = "Hello";
  char recvMsg[MAX_LENGTH_MSG];

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  server.sin_family = AF_INET;
  server.sin_port = htons(8080);
  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);
  server.sin_addr = ip;

  if(bind(fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
  {
    close(fd);
    perror("Error bind");
    exit(EXIT_FAILURE);
  }

  listen(fd, MAX_LENGTH_QUEUE_CLIENTS);

  socklen_t sockClientLen;
  int newFd = accept(fd, (struct sockaddr*)&client, &sockClientLen);
  if(newFd == -1)
  {
    close(fd);
    perror("Error create new socket");
    exit(EXIT_FAILURE);
  }

  recv(newFd, recvMsg, MAX_LENGTH_MSG, 0);
  send(newFd, sendMsg, strlen(sendMsg)+1, 0);

  printf("%s\n", recvMsg);

  close(newFd);
  close(fd);

  return 0;
}
