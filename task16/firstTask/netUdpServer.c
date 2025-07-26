#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_LENGTH_QUEUE_CLIENTS 1
#define MAX_LENGTH_MSG 20
#define PORT_SERVER 7777
#define PORT_CLIENT 7778
int main()
{
  struct sockaddr_in server, client;
  char *sendMsg = "Hello";
  char recvMsg[MAX_LENGTH_MSG];

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);

  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT_SERVER);
  server.sin_addr = ip;

  memset(&client, 0, sizeof(server));
  client.sin_family = AF_INET;
  client.sin_port = htons(PORT_CLIENT);
  client.sin_addr = ip;

  if(bind(fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
  {
    close(fd);
    perror("Error bind");
    exit(EXIT_FAILURE);
  }

  socklen_t clientLen = sizeof(client);
  int bytes = recvfrom(fd, recvMsg, MAX_LENGTH_MSG, 0, (struct sockaddr *)&client, &clientLen);
  if(bytes <= 0)
  {
    close(fd);
    perror("Error recv");
    exit(EXIT_FAILURE);
  }

  if(sendto(fd, sendMsg, strlen(sendMsg)+1, 0, (struct sockaddr *)&client, clientLen) == -1)
  {
    close(fd);
    perror("Error send");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", recvMsg);

  close(fd);

  return 0;
}
