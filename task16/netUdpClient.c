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
  struct sockaddr_in server, client;
  char *sendMsg = "Hi";
  char recvMsg[MAX_LENGTH_MSG];

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);

  client.sin_family = AF_INET;
  client.sin_port = htons(8082);
  client.sin_addr = ip;

  if(bind(fd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) == -1)
  {
    close(fd);
    perror("Error bind");
    exit(EXIT_FAILURE);
  }

  server.sin_family = AF_INET;
  server.sin_port = htons(8081);
  server.sin_addr = ip;

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
