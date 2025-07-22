#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/epoll.h>

#define MAX_LENGTH_MSG 32
#define COUNT_SERVERS 256
#define COUNT_EVENTS 256
#define MAX_LENGTH_QUEUE_CLIENTS 5

int fdTcp = 0;
int fdUdp = 0;
pthread_t servers[COUNT_SERVERS];

void handleTcp()
{
  char timeStr[MAX_LENGTH_MSG];
  struct sockaddr_in client;
  socklen_t clientLen = sizeof(client);

  int newFd = accept(fdTcp, (struct sockaddr *)&client, &clientLen);

  time_t myTime = time(NULL);
  struct tm *now = localtime(&myTime);
  snprintf(timeStr, MAX_LENGTH_MSG, "Time %d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);

  send(newFd, timeStr, strlen(timeStr)+1, 0);
  close(newFd);
}
void handleUdp()
{
  char timeStr[MAX_LENGTH_MSG];
  char buf[3];
  struct sockaddr_in client;
  socklen_t clientLen = sizeof(client);

  time_t myTime = time(NULL);
  struct tm *now = localtime(&myTime);
  snprintf(timeStr, MAX_LENGTH_MSG, "Time %d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);

  recvfrom(fdUdp, buf, 3, 0, (struct sockaddr *)&client, &clientLen);
  sendto(fdUdp, timeStr, strlen(timeStr)+1, 0, (struct sockaddr *)&client, clientLen);

}
void handlerSignal(int sig)
{
  close(fdTcp);
  close(fdUdp);
  exit(EXIT_SUCCESS);
}
int main()
{
  struct sigaction sigact;
  sigact.sa_handler = handlerSignal;

  sigaction(SIGTERM, &sigact, NULL);
  sigaction(SIGINT, &sigact, NULL);

  struct sockaddr_in serverTcp, serverUdp;

  fdTcp = socket(AF_INET, SOCK_STREAM, 0);
  if(fdTcp == -1)
  {
    perror("Error create fd");
    exit(EXIT_FAILURE);
  }
  fdUdp = socket(AF_INET, SOCK_DGRAM, 0);
  if(fdUdp == -1)
  {
    perror("Error create fd");
    close(fdTcp);
    exit(EXIT_FAILURE);
  }

  int optval = 1;
  if(setsockopt(fdTcp, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
  {
    perror("Error set socket option");
    close(fdTcp);
    close(fdUdp);
    exit(EXIT_FAILURE);
  }
  if(setsockopt(fdUdp, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
  {
    perror("Error set socket option");
    close(fdTcp);
    close(fdUdp);
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);

  memset(&serverTcp, 0, sizeof(serverTcp));
  serverTcp.sin_family = AF_INET;
  serverTcp.sin_port = htons(7777);
  serverTcp.sin_addr = ip;

  memset(&serverUdp, 0, sizeof(serverUdp));
  serverUdp.sin_family = AF_INET;
  serverUdp.sin_port = htons(7778);
  serverUdp.sin_addr = ip;

  if(bind(fdTcp, (struct sockaddr *)&serverTcp, sizeof(serverTcp)) ==  -1)
  {
    close(fdTcp);
    close(fdUdp);
    perror("Error server create bind tcp");
    exit(EXIT_FAILURE);
  }
  if(bind(fdUdp, (struct sockaddr *)&serverUdp, sizeof(serverUdp)) ==  -1)
  {
    close(fdTcp);
    close(fdUdp);
    perror("Error server create bind udp");
    exit(EXIT_FAILURE);
  }

  if(listen(fdTcp, MAX_LENGTH_QUEUE_CLIENTS))
  {
    close(fdTcp);
    close(fdUdp);
    perror("Error server listen");
    exit(EXIT_FAILURE);
  }

  int epollFd = epoll_create1(0);
  struct epoll_event event;
  event.events = EPOLLIN;

  event.data.fd = fdTcp;
  epoll_ctl(epollFd, EPOLL_CTL_ADD, fdTcp, &event);

  event.data.fd = fdUdp;
  epoll_ctl(epollFd, EPOLL_CTL_ADD, fdUdp, &event);

  struct epoll_event events[COUNT_EVENTS];
  for(;;)
  {
    int countEvents = epoll_wait(epollFd, events, COUNT_EVENTS, -1);

    for(int i = 0; i < countEvents; i++)
    {
      int fd = events[i].data.fd;
      if(fd == fdTcp)
      {
        handleTcp();
      }
      else if(fd == fdUdp)
      {
        handleUdp();
      }
    }
  }
  return 0;
}
