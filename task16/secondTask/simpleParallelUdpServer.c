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
#define MAX_LENGTH_QUEUE_CLIENTS 5

int fdMainServer= 0;
struct PthreadArgs 
{
  struct sockaddr_in server;

  struct sockaddr_in client;
  socklen_t clientLen;
};

void *handleClient(void *pthreadArgs)
{
  char timeStr[MAX_LENGTH_MSG];
  struct PthreadArgs *args = (struct PthreadArgs *) pthreadArgs;
  struct sockaddr_in newServer = (struct sockaddr_in) args->server;
  struct sockaddr_in client = (struct sockaddr_in) args->client;
  socklen_t clientLen = args->clientLen;

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd == -1)
  {
    perror("Error create fdMainServer");
    exit(EXIT_FAILURE);
  }


  if(bind(fd, (struct sockaddr *)&newServer, sizeof(struct sockaddr_in)) ==  -1)
  {
    close(fd);
    perror("Error sever create bind");
    exit(EXIT_FAILURE);
  }

  time_t myTime = time(NULL);
  struct tm *now = localtime(&myTime);
  snprintf(timeStr, MAX_LENGTH_MSG, "Time %d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);

  sendto(fd, timeStr, strlen(timeStr)+1, 0, (struct sockaddr *)&client, clientLen);

  free(pthreadArgs);
  close(fd);

  return NULL;
}
void handlerSignal(int sig)
{
  close(fdMainServer);
  exit(EXIT_SUCCESS);
}
int main()
{
  struct sigaction sigact;
  sigact.sa_handler = handlerSignal;

  sigaction(SIGTERM, &sigact, NULL);
  sigaction(SIGINT, &sigact, NULL);

  struct sockaddr_in server;
  fdMainServer = socket(AF_INET, SOCK_DGRAM, 0);
  if(fdMainServer == -1)
  {
    perror("Error create fdMainServer");
    exit(EXIT_FAILURE);
  }

  int optval = 0;
  if(setsockopt(fdMainServer, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
  {
    perror("Error set socket option");
    close(fdMainServer);
    exit(EXIT_FAILURE);
  }

  int port = 7777;
  struct in_addr ip;
  inet_pton(AF_INET, "127.0.0.1", &ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr = ip;

  if(bind(fdMainServer, (struct sockaddr *)&server, sizeof(server)) ==  -1)
  {
    close(fdMainServer);
    perror("Error mainServer create bind");
    exit(EXIT_FAILURE);
  }

  for(;;)
  {
    struct sockaddr_in client;
    socklen_t clientLen = sizeof(client);
    char recvMsg[10];
    recvfrom(fdMainServer, recvMsg, MAX_LENGTH_MSG, 0, (struct sockaddr *)&client, &clientLen);

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

    pthread_t newServerThread;
    if(pthread_create(&newServerThread, NULL, handleClient, (void*)pthreadArgs) != 0)
    {
      free(pthreadArgs);
      continue;
    }

    pthread_detach(newServerThread);
  }
  return 0;
}
