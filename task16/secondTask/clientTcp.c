#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_LENGTH_MSG 32
#define PORT_SERVER 7777
int main()
{
  struct sockaddr_in server;
  char rcvMsg[MAX_LENGTH_MSG];
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd == -1)
  {
    perror("Error create fd");
    exit(EXIT_FAILURE);
  }

  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
  if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
  {
    perror("Error set socket option");
    close(fd);
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT_SERVER);
  server.sin_addr = ip;

  if(connect(fd, (struct sockaddr *)&server, sizeof(server)) == -1)
  {
    close(fd);
    perror("Error create connect");
    exit(EXIT_FAILURE);
  }

  int bytes = recv(fd, rcvMsg, MAX_LENGTH_MSG, 0);
  if(bytes <= 0)
  {
    close(fd);
    perror("Error recv");
    exit(EXIT_FAILURE);
  }

  printf("Ip server: %s, Port: %d\n", inet_ntoa(server.sin_addr),ntohs(server.sin_port));
  printf("%s\n", rcvMsg);

  close(fd);

  return 0;
}
