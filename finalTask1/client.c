#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "rawUdp.h"

#define PORT_SERVER 7777
#define IP_SERVER "127.0.0.1"
#define MAX_LENGTH_MSG 64

int fd = 0;
uint16_t createRandPort()
{
  srand(time(NULL));
  return 10000 + (rand() % 55535);
}
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

  struct sockaddr_in server;
  uint8_t rcvPacket[MAX_LENGTH_PACKET];
  uint8_t rcvStr[MAX_LENGTH_MSG];
  uint8_t sndPacket[MAX_LENGTH_PACKET];
  char data[MAX_LENGTH_MSG];
  uint16_t destPortRcvPacket = 0;
  int length = 0;
  uint16_t portServer = PORT_SERVER;
  uint16_t port = createRandPort();

  fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if(fd == -1)
  {
    perror("Error client create fd");
    exit(EXIT_FAILURE);
  }

  struct in_addr ip; 
  inet_pton(AF_INET, IP_SERVER, &ip);
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr = ip;

  struct sockaddr_in from;
  socklen_t socklen;

  length = createPacket(sndPacket, "init", port, PORT_SERVER);
  if(sendto(fd, sndPacket, length, 0, (struct sockaddr *)&server, sizeof(server)) == -1)
  {
    perror("Error send");
    close(fd);
    exit(EXIT_FAILURE);
  }
  do
  {
    int bytes = recvfrom(fd, rcvPacket, MAX_LENGTH_PACKET, 0, (struct sockaddr *)&from, &socklen);
    if(bytes <= 0)
    {
      perror("Error recv");
      close(fd);
      exit(EXIT_FAILURE);
    }
    memcpy(&destPortRcvPacket, &rcvPacket[IP_HEADER_OFFSET+2], sizeof(destPortRcvPacket));
  } while(ntohs(destPortRcvPacket) != port);
  printData(rcvPacket);
      memcpy(&portServer, &rcvPacket[IP_HEADER_OFFSET], sizeof(portServer));

  for(;;)
  {
    printf("Enter the message\n");
    fgets(data, MAX_LENGTH_MSG, stdin);
    data[strlen(data)-1] = '\0';

    length = createPacket(sndPacket, data, port, portServer);

    if(sendto(fd, sndPacket, length, 0, (struct sockaddr *)&from, sizeof(from)) == -1)
    {
      perror("Error send");
      close(fd);
      exit(EXIT_FAILURE);
    }

    do
    {
      int bytes = recvfrom(fd, rcvPacket, MAX_LENGTH_PACKET, 0, (struct sockaddr *)&from, &socklen);
      if(bytes <= 0)
      {
        perror("Error recv");
        close(fd);
        exit(EXIT_FAILURE);
      }
      memcpy(&destPortRcvPacket, &rcvPacket[IP_HEADER_OFFSET+2], sizeof(destPortRcvPacket));
    } while(ntohs(destPortRcvPacket) != port);

    printData(rcvPacket);
  }

  close(fd);

  return 0;
}
