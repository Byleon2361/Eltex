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
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error client create fd");
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
  server.sin_family = AF_INET;
  server.sin_port = htons(7777);
  server.sin_addr = ip;

  struct sockaddr_in from;
  socklen_t sockLen;
  sendto(fd, "Hi", 3, 0, (struct sockaddr *)&server, sizeof(server));
  if(recvfrom(fd, rcvMsg, MAX_LENGTH_MSG, 0, (struct sockaddr *)&from, &sockLen) == -1)
  {
    exit(EXIT_FAILURE);
  }

  printf("Ip server: %s, Port: %d\n", inet_ntoa(from.sin_addr),ntohs(from.sin_port));
  printf("%s\n", rcvMsg);

  close(fd);

  return 0;
}
