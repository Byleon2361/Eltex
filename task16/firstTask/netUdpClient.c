#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT_SERVER 7777
#define PORT_CLIENT 7778
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

  memset(&client, 0, sizeof(client));
  client.sin_family = AF_INET;
  client.sin_port = htons(PORT_CLIENT);
  client.sin_addr = ip;

  if(bind(fd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) == -1)
  {
    close(fd);
    perror("Error bind");
    exit(EXIT_FAILURE);
  }

  memset(&server, 0, sizeof(client));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT_SERVER);
  server.sin_addr = ip;

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
