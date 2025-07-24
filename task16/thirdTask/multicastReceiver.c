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
  struct in_addr ipGroupAddr;
  struct in_addr ipLocalAddr;
  inet_pton(AF_INET, "224.0.0.1", &ipGroupAddr);
  inet_pton(AF_INET, "0.0.0.0", &ipLocalAddr);
  memset(&receiver, 0, sizeof(receiver));
  receiver.sin_family = AF_INET;
  receiver.sin_addr = ipLocalAddr;
  receiver.sin_port = htons(7777);

  struct ip_mreqn mreqn;
  mreqn.imr_multiaddr = ipGroupAddr;
  mreqn.imr_address = ipLocalAddr;

  if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof(mreqn)) == -1)
  {
    perror("Error create socket");
    close(fd);
    exit(EXIT_FAILURE);
  }

  if(bind(fd, (struct sockaddr *)&receiver, (socklen_t)sizeof(receiver)))
  {
    perror("Error create socket");
    close(fd);
    exit(EXIT_FAILURE);
  }
  recv(fd, msg, MAX_LEN_MSG, 0);
  printf("%s\n", msg);
  close(fd);

  return 0;
}
