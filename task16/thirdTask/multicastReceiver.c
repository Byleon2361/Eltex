#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <net/if.h>
#define MAX_LEN_MSG 32
#define MAX_LEN_ARG 12
int main(int argc, char *argv[])
{
  char interface[MAX_LEN_ARG]; 
  printf("argc %d\n", argc);
  if(argc > 2)
  {
    fprintf(stderr, "Error: Too many args\n");
    exit(EXIT_FAILURE);
  }
  else if (argc == 2) 
  {
    strncpy(interface, argv[1], MAX_LEN_ARG);
  }

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
  mreqn.imr_ifindex = (argc == 2) ? if_nametoindex(interface) : 0;

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
