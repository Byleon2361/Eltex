#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define MAX_LEN_MSG 32
int main()
{
  struct sockaddr_in receiver;
  char msg[MAX_LEN_MSG];
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }
  struct in_addr ip;
  inet_pton(AF_INET, "255.255.255.255", &ip);
  memset(&receiver, 0, sizeof(receiver));
  receiver.sin_family = AF_INET;
  receiver.sin_addr = ip;
  receiver.sin_port = htons(7777);

  if(bind(fd, (struct sockaddr *)&receiver, (socklen_t)sizeof(receiver)))
  {
    perror("Error create socket");
    close(fd);
    exit(EXIT_FAILURE);
  }
  recv(fd, msg, MAX_LEN_MSG, 0);
  printf("%s\n", msg);

  return 0;
}
