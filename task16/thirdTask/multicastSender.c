#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
int main()
{
  struct sockaddr_in sender;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  inet_pton(AF_INET, "224.0.0.1", &ip);
  memset(&sender, 0, sizeof(sender));
  sender.sin_family = AF_INET;
  sender.sin_addr = ip;
  sender.sin_port = htons(7777);

  char *msg = "Hi";
  sendto(fd, msg, strlen(msg), 0,(struct sockaddr *)&sender, (socklen_t)sizeof(sender));

  close(fd);
  return 0;
}
