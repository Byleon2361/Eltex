#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LENGTH_MSG 20
#define PORT_SERVER 7778
#define MAX_LEN_MSG 65535
#define IP_SERVER "192.168.56.2"

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
  inet_pton(AF_INET, IP_SERVER, &ip);

  memset(&server, 0, sizeof(server));
  memset(&client, 0, sizeof(client));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT_SERVER);
  server.sin_addr = ip;


  if(bind(fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
  {
    close(fd);
    perror("Error bind");
    exit(EXIT_FAILURE);
  }

  socklen_t clientLen = sizeof(client);
  for(;;)
  {
    int bytes = recvfrom(fd, recvMsg, MAX_LENGTH_MSG, 0, (struct sockaddr *)&client, &clientLen);
    if(bytes <= 0)
    {
      close(fd);
      perror("Error recv");
      exit(EXIT_FAILURE);
    }
    printf("%s\n", recvMsg);
    if(sendto(fd, sendMsg, strlen(sendMsg)+1, 0, (struct sockaddr *)&client, clientLen) == -1)
    {
      close(fd);
      perror("Error send");
      exit(EXIT_FAILURE);
    }
  }

  close(fd);

  return 0;
}
