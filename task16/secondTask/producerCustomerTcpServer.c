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
#define COUNT_SERVERS 256
#define MAX_LENGTH_QUEUE_CLIENTS 5

int fd = 0;
pthread_t servers[COUNT_SERVERS];

int tasks[COUNT_SERVERS];
int countTasks;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void taskPush(int arg)
{
  tasks[countTasks] = arg;
  countTasks++;
}
int taskPop()
{
  if(countTasks <= 0)
  {
    fprintf(stderr, "Error pop - countTasks = 0\n");
    return -1;
  }
  
  int res = tasks[0];
  countTasks--;
  for(int i = 0; i < countTasks; i++)
  {
    tasks[i] = tasks[i+1];
  }

  return res;
}

void *handleClient(void *serverArg)
{
  char timeStr[MAX_LENGTH_MSG];
  for(;;)
  {
    pthread_mutex_lock(&m);
    pthread_cond_wait(&cond, &m);
    int newFd = taskPop();
    pthread_mutex_unlock(&m);
    if(newFd == -1)
    {
      close(newFd);
      return NULL;
    }

    time_t myTime = time(NULL);
    struct tm *now = localtime(&myTime);
    snprintf(timeStr, MAX_LENGTH_MSG, "Time %d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);

    send(newFd, timeStr, strlen(timeStr)+1, 0);
    close(newFd);
  }

  return NULL;
}
void handlerSignal(int sig)
{
  for(int i =0; i < COUNT_SERVERS; i++)
  {
    pthread_cancel(servers[i]);
  }
  close(fd);
  exit(EXIT_SUCCESS);
}
int main()
{
  struct sigaction sigact;
  sigact.sa_handler = handlerSignal;

  sigaction(SIGTERM, &sigact, NULL);

  struct sockaddr_in server;
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

  for(int i = 0; i < COUNT_SERVERS; i++)
  {
    if(pthread_create(&servers[i], NULL, handleClient, NULL) != 0)
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
    pthread_mutex_lock(&m);
    taskPush(newFd);
    pthread_mutex_unlock(&m);
    pthread_cond_signal(&cond);
  }
  return 0;
}
