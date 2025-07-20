#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#define MAX_LENGTH_MSG 32
#define MAX_COUNT_THREADS 256
#define MAX_LENGTH_QUEUE_CLIENTS 5

int fd = 0;
struct server
{
  pthread_t serverThread;
  int newFd;
  int isUsing;
};
struct server servers[MAX_COUNT_THREADS];

void *handleClient(void *serverArg)
{
  char timeStr[MAX_LENGTH_MSG];
  struct server *server = (struct server *)serverArg;

  for(;;)
  {
    if(server->newFd == -1)
    {
      server->isUsing = 0;
      close(server->newFd);
      return NULL;
    }

    time_t myTime = time(NULL);
    struct tm *now = localtime(&myTime);
    snprintf(timeStr, MAX_LENGTH_MSG, "Time %d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);

    send(server->newFd, timeStr, strlen(timeStr)+1, 0);
    close(server->newFd);
    server->isUsing = 0;
  }

  return NULL;
}
void handlerSignal(int sig)
{
  for(int i =0; i < MAX_COUNT_THREADS; i++)
  {
    pthread_cancel(servers[i].serverThread);
  }
  close(fd);
  exit(EXIT_SUCCESS);
}
int main()
{
  struct sigaction sigact;
  sigact.sa_handler = handlerSignal;

  sigaction(SIGTERM, &sigact, NULL);

  struct sockaddr_in server, client;
  socklen_t lenAddr = sizeof(struct sockaddr_in);
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd == -1)
  {
    perror("Error create fd");
    exit(EXIT_FAILURE);
  }
  int optval = 1;
  if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
  {
    perror("Error set socket option");
    close(fd);
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(7777);
  server.sin_addr = ip;

  if(bind(fd, (struct sockaddr *)&server, sizeof(server)) ==  -1)
  {
    close(fd);
    perror("Error server create bind");
    exit(EXIT_FAILURE);
  }

  listen(fd, MAX_LENGTH_QUEUE_CLIENTS);

  for(int i = 0; i < MAX_COUNT_THREADS; i++)
  {
    servers[i].isUsing = 0;
    if(pthread_create(&servers[i].serverThread, NULL, handleClient, (void*)&servers[i]) != 0)
    {
      close(fd);
      perror("Error create thread");
      exit(EXIT_FAILURE);
    }
  }
  for(;;)
  {
    struct sockaddr_in client;
    socklen_t clientLen = sizeof(client);
    int newFd = accept(fd, (struct sockaddr *)&client, &clientLen);

    for(int i = 0; i < MAX_COUNT_THREADS; i++)
    {
      if(!servers[i].isUsing)
      {
        servers[i].newFd = newFd;
        servers[i].isUsing = 1;
        break;
      }
    }
  }
  return 0;
}
