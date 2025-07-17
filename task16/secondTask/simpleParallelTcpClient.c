#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_LENGTH_QUEUE_CLIENTS 1
#define MAX_LENGTH_MSG 32
int main()
{
  struct sockaddr_in server;
  char rcvMsg[32];
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd == -1)
  {
    perror("Error create fd");
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(7777);
  server.sin_addr = ip;

  if(connect(fd, (struct sockaddr *)&server, sizeof(server)) == -1)
  {
    close(fd);
    perror("Error create connect");
    exit(EXIT_FAILURE);
  }

  recv(fd, rcvMsg, MAX_LENGTH_MSG, 0);

  printf("%s\n", rcvMsg);

  close(fd);

  return 0;
}
