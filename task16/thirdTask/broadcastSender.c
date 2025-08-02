#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORT 7777
int main()
{
  struct sockaddr_in sender;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }
  int optval = 1;
  if(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1)
  {
    perror("Error create socket");
    close(fd);
    exit(EXIT_FAILURE);
  }
  struct in_addr ip;
  inet_pton(AF_INET, "255.255.255.255", &ip);
  memset(&sender, 0, sizeof(sender));
  sender.sin_family = AF_INET;
  sender.sin_addr = ip;
  sender.sin_port = htons(PORT);

  char *msg = "Hi";
  if(sendto(fd, msg, strlen(msg), 0,(struct sockaddr *)&sender, (socklen_t)sizeof(sender)) == -1)
  {
    perror("Error send");
    close(fd);
    exit(EXIT_FAILURE);
  }

  close(fd);
  return 0;
}
