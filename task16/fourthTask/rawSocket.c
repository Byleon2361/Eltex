#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAX_LEN_MSG 65535
int fd = 0;
void handlerSignal(int sig)
{
  close(fd);
  exit(EXIT_SUCCESS);
}
int main()
{
  struct sigaction sigact;
  sigact.sa_handler = handlerSignal;

  sigaction(SIGTERM, &sigact, NULL);
  sigaction(SIGINT, &sigact, NULL);

  unsigned char buf[MAX_LEN_MSG];
  fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if(fd == -1)
  {
    perror("Error create sokect");
    exit(EXIT_FAILURE);
  }

  printf("Udp sniffer started\n");
  for(;;)
  {
    int bytes = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
    if(bytes <= 0)
    {
      perror("Error recv");
      continue;
    }
    printf("--------------------------\n");
    printf("Recv packet %d bytes\n", bytes);
    for(int i = 0; i < bytes; i++)
    {
      printf("%02x ", buf[i]);
      if(i % 15 == 0) printf("\n");
    }
    printf("\n--------------------------\n");
  }
  return 0;
}
