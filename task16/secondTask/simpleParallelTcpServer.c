#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_LENGTH_QUEUE_CLIENTS 1
#define MAX_LENGTH_MSG 32
void *handleClient(void *newFdVoid)
{
  char timeStr[MAX_LENGTH_MSG];
  int *newFd = (int*)newFdVoid;

  time_t myTime = time(NULL);
  struct tm *now = localtime(&myTime);
  snprintf(timeStr, MAX_LENGTH_MSG, "Time %d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);

  send(*newFd, timeStr, strlen(timeStr)+1, 0);

  return NULL;
}
int main()
{
  struct sockaddr_in server, client;
  socklen_t lenAddr = sizeof(struct sockaddr_in*);
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd == -1)
  {
    perror("Error create fd");
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(7777);
  server.sin_addr = ip;

  if(bind(fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) ==  -1)
  {
    close(fd);
    perror("Error create bind");
    exit(EXIT_FAILURE);
  }

  listen(fd, MAX_LENGTH_QUEUE_CLIENTS);

  for(;;)
  {
    int newFd = accept(fd, (struct sockaddr *)&client, &lenAddr);
    if(newFd < 0) continue;

    pthread_t newServer;
    if(pthread_create(&newServer, NULL, handleClient, (void*)&newFd) != 0) continue;

    pthread_detach(newServer);
  }
  return 0;
}
