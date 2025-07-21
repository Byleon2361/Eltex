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

int fdMain = 0;
struct server
{
  pthread_t serverThread;
  struct sockaddr_in client;
  int isUsing;
};
struct server servers[MAX_COUNT_THREADS];

void *handleClient(void *serverArg)
{
  char timeStr[MAX_LENGTH_MSG];
  struct server *server = (struct server *)serverArg;

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error create fd");
    exit(EXIT_FAILURE);
  }

  for(;;)
  {
    time_t myTime = time(NULL);
    struct tm *now = localtime(&myTime);
    snprintf(timeStr, MAX_LENGTH_MSG, "Time %d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);

    sendto(fd, timeStr, strlen(timeStr)+1, 0, (struct sockaddr *)&server->client, sizeof(server->client));
    server->isUsing = 0;
  }
  close(fd);
  return NULL;
}
void handlerSignal(int sig)
{
  for(int i =0; i < MAX_COUNT_THREADS; i++)
  {
    pthread_cancel(servers[i].serverThread);
  }
  close(fdMain);
  exit(EXIT_SUCCESS);
}
int main()
{
  struct sigaction sigact;
  sigact.sa_handler = handlerSignal;

  sigaction(SIGTERM, &sigact, NULL);

  struct sockaddr_in server;

  fdMain = socket(AF_INET, SOCK_DGRAM, 0);
  if(fdMain == -1)
  {
    perror("Error create fd");
    exit(EXIT_FAILURE);
  }
  int optval = 1;
  if(setsockopt(fdMain, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
  {
    perror("Error set socket option");
    close(fdMain);
    exit(EXIT_FAILURE);
  }

  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(7777);
  server.sin_addr = ip;

  if(bind(fdMain, (struct sockaddr *)&server, sizeof(server)) ==  -1)
  {
    close(fdMain);
    perror("Error server create bind");
    exit(EXIT_FAILURE);
  }

  for(int i = 0; i < MAX_COUNT_THREADS; i++)
  {
    servers[i].isUsing = 0;
    if(pthread_create(&servers[i].serverThread, NULL, handleClient, (void*)&servers[i]) != 0)
    {
      close(fdMain);
      perror("Error create thread");
      exit(EXIT_FAILURE);
    }
  }
  for(;;)
  {
    struct sockaddr_in client;
    socklen_t clientLen = sizeof(client);
    char buf[3];
    recvfrom(fdMain, buf, 3, 0, (struct sockaddr *)&client, &clientLen);

    for(int i = 0; i < MAX_COUNT_THREADS; i++)
    {
      if(!servers[i].isUsing)
      {
        servers[i].client = client;
        servers[i].isUsing = 1;
        break;
      }
    }
  }
  return 0;
}
