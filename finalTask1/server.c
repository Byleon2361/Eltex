#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/epoll.h>
#include "rawUdp.h"
#define PORT_SERVER 7777
#define IP_SERVER "127.0.0.1"

int fdMain = 0;
struct PthreadArgs
{
  struct sockaddr_in server;

  struct sockaddr_in client;
  socklen_t clientLen;

  int portSrc;
  int clientPort;
};
void exitNoticeClient(int fd, uint16_t portSrc, uint16_t clientPort, struct sockaddr_in *client, int clientLen)
{
  uint8_t sndPacket[MAX_LENGTH_PACKET];
  int length = createPacket(sndPacket, "fatal", portSrc, clientPort);
  if(sendto(fd, sndPacket, length, 0, (struct sockaddr *)&client, clientLen) == -1)
  {
    close(fdMain);
    perror("Error send");
    exit(EXIT_FAILURE);
  }
}
void *handleClient(void *pthreadArgs)
{
  struct PthreadArgs *args = (struct PthreadArgs *) pthreadArgs;
  struct sockaddr_in newServer = (struct sockaddr_in) args->server;
  struct sockaddr_in client = (struct sockaddr_in) args->client;
  socklen_t clientLen = args->clientLen;

  uint16_t srcPortRcvPacket = 0;
  char sendMsg[MAX_LENGTH_MSG];
  char recvMsg[MAX_LENGTH_MSG];
  uint8_t rcvPacket[MAX_LENGTH_PACKET];
  uint8_t sndPacket[MAX_LENGTH_PACKET];
  int index = 1;
  int length = 0;

  int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if(fd == -1)
  {
    perror("Error create fd");
    exitNoticeClient(fd, args->portSrc, args->clientPort, &args->client, args->clientLen);
    close(fdMain);
    exit(EXIT_FAILURE);
  }

  length = createPacket(sndPacket, "init", args->portSrc, args->clientPort);
  if(sendto(fd, sndPacket, length, 0, (struct sockaddr *)&client, sizeof(client)) == -1)
  {
    perror("Error send");
    exitNoticeClient(fd, args->portSrc, args->clientPort, &args->client, args->clientLen);
    close(fd);
    exit(EXIT_FAILURE);
  }

  fd_set readfd;
  struct timeval tv;
  int retval;

  for(;;)
  {
    FD_ZERO(&readfd);
    FD_SET(fd, &readfd);

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    retval = select(fd+1, &readfd, NULL, NULL, &tv);
    if(retval == -1)
    {
      perror("Failed select");
      exitNoticeClient(fd, args->portSrc, args->clientPort, &args->client, args->clientLen);
      close(fdMain);
      exit(EXIT_FAILURE);
    }
    else if(retval == 0)
    {
      perror("Too long time waiting");
      exitNoticeClient(fd, args->portSrc, args->clientPort, &args->client, args->clientLen);
      close(fdMain);
      exit(EXIT_FAILURE);
    }

    int bytes = recvfrom(fd, rcvPacket, MAX_LENGTH_PACKET, 0, (struct sockaddr *)&client, &clientLen);
    if(bytes <= 0)
    {
      perror("Error recv");
      exitNoticeClient(fd, args->portSrc, args->clientPort, &args->client, args->clientLen);
      close(fdMain);
      exit(EXIT_FAILURE);
    }
    memcpy(&srcPortRcvPacket, &rcvPacket[IP_HEADER_OFFSET], sizeof(srcPortRcvPacket));
    if(ntohs(srcPortRcvPacket) != args->clientPort) continue;

    extractData(rcvPacket, recvMsg);

    snprintf(sendMsg, MAX_LENGTH_MSG, "%s %d", recvMsg, index);
    index++;

    length = createPacket(sndPacket, sendMsg, args->portSrc, args->clientPort);

    if(sendto(fd, sndPacket, length, 0, (struct sockaddr *)&client, clientLen) == -1)
    {
      close(fdMain);
      exitNoticeClient(fd, args->portSrc, args->clientPort, &args->client, args->clientLen);
      perror("Error send");
      exit(EXIT_FAILURE);
    }
    printf("%s\n", recvMsg);
  }

  free(pthreadArgs);
  close(fd);

  return NULL;
}
void handlerSignal(int sig)
{
  close(fdMain);
  exit(EXIT_SUCCESS);
}
int main()
{
  struct sigaction sigact;
  sigact.sa_handler = handlerSignal;

  sigaction(SIGTERM, &sigact, NULL);
  sigaction(SIGINT, &sigact, NULL);

  struct sockaddr_in server, client;

  fdMain = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if(fdMain == -1)
  {
    perror("Error create socket");
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  int port = PORT_SERVER;
  inet_pton(AF_INET, IP_SERVER, &ip);

  memset(&server, 0, sizeof(server));
  memset(&client, 0, sizeof(client));
  server.sin_family = AF_INET;
  server.sin_addr = ip;

  if(bind(fdMain, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1)
  {
    close(fdMain);
    perror("Error bind");
    exit(EXIT_FAILURE);
  }

  uint8_t rcvPacket[MAX_LENGTH_PACKET];
  socklen_t clientLen = sizeof(client);
  uint16_t destPortRcvPacket = 0;
  uint16_t clientPortTemp = 0;
  uint16_t clientPort = 0;
  for(;;)
  { 
    do
    {
      int bytes = recvfrom(fdMain, rcvPacket, MAX_LENGTH_PACKET, 0, (struct sockaddr *)&client, &clientLen);
      if(bytes <= 0)
      {
        perror("Error recv");
        close(fdMain);
        exit(EXIT_FAILURE);
      }
      memcpy(&destPortRcvPacket, &rcvPacket[IP_HEADER_OFFSET+2], sizeof(destPortRcvPacket));
    } while(ntohs(destPortRcvPacket) != PORT_SERVER);
    printData(rcvPacket);
    printf("create new server\n");

    memcpy(&clientPortTemp, &rcvPacket[IP_HEADER_OFFSET], sizeof(clientPortTemp));
    clientPort = ntohs(clientPortTemp);

    struct sockaddr_in newServer;
    port++;
    if(port > 65535) port = 7778;
    newServer.sin_family = AF_INET;
    newServer.sin_port = htons(port);
    newServer.sin_addr = ip;

    struct PthreadArgs *pthreadArgs = malloc(sizeof(struct PthreadArgs));
    pthreadArgs->server = newServer;
    pthreadArgs->client = client;
    pthreadArgs->clientLen = clientLen;
    pthreadArgs->portSrc = port;
    pthreadArgs->clientPort = clientPort;

    pthread_t newServerThread;
    if(pthread_create(&newServerThread, NULL, handleClient, (void*)pthreadArgs) != 0)
    {
      free(pthreadArgs);
      continue;
    }

    pthread_detach(newServerThread);
  }

  close(fdMain);

  return 0;
}
