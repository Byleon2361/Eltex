#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define MAX_COUNT_MESSAGES 8
#define MAX_MSG_SIZE 20

#define NICKNAME_PRIO 2
#define CLEAR_PRIO 3
#define DIED_PRIO 4
#define MAX_LENGTH_NICKNAME 20
#define MAX_COUNT_NICKNAMES 16
#define MSG_PRIO 1
#define MAX_LENGTH_MSG 256
#define MAX_COUNT_MSGS 50

typedef struct client
{
  char nickname[MAX_LENGTH_NICKNAME];
  int isActive;
} Client; 
Client clients[MAX_COUNT_NICKNAMES]; 
int countClients = 0;
int countMsgs = 0;

void cleanClients()
{
  int i =0;
  char bufNick[MAX_LENGTH_NICKNAME+1];
  for(int j = 0; j<countClients; j++)
  {
    snprintf(bufNick, sizeof(bufNick), "/%s", clients[j].nickname);
    mqd_t client = mq_open(bufNick, O_WRONLY);
    if (client == -1)
    {
      printf("client %s - DIE\n", clients[j].nickname);
      clients[j].isActive = 0;
    }
    else
    {
      mq_close(client);
    }
  }
  while(i<countClients)
  {
    if(!clients[i].isActive)
    {
      for(int j =i; j<countClients-1; j++)
      {
        clients[j] = clients[j+1];
      }
      countClients--;
    }
    else
    {
      i++;
    }
  }
}
void addClient(char *nickname)
{
  if(countClients >= MAX_COUNT_NICKNAMES)
  {
    perror("Failed - max count clients");
    exit(EXIT_FAILURE);
  }

  strncpy(clients[countClients].nickname, nickname, MAX_LENGTH_NICKNAME);
  clients[countClients].isActive = 1;
  countClients++;
  return;
}
void broadcastNicknames()
{
    printf("COUNT CLIENTS %d\n", countClients);
  char bufNick[MAX_LENGTH_NICKNAME+1];
  for(int i = 0; i<countClients; i++)
  {
    snprintf(bufNick, sizeof(bufNick), "/%s", clients[i].nickname);
    mqd_t client = mq_open(bufNick, O_WRONLY);
    if (client == -1)
    {
      clients[i].isActive = 0;
    }

    mq_send(client, "\0", 2, CLEAR_PRIO);
    for(int j = 0; j<countClients; j++)
    {
      mq_send(client, clients[j].nickname, strlen(clients[j].nickname) + 1, NICKNAME_PRIO);
      printf("%s\n", clients[j].nickname);
    }
    mq_close(client);

    printf("%s\n", bufNick);
  }
  printf("------\n");
}
void *nicknameMain(void* args)
{

  struct mq_attr attr;
  attr.mq_maxmsg = MAX_COUNT_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;

  mqd_t serviceServerQueue = mq_open("/serviceServerQueue", O_RDONLY | O_CREAT, 0600, &attr);
  if (serviceServerQueue == -1)
  {
    perror("Failed create queue");
    exit(EXIT_FAILURE);
  }

  char clientName[MAX_LENGTH_NICKNAME];
  unsigned int prio;
  while(1)
  {
    if(mq_receive(serviceServerQueue, clientName, sizeof(clientName), &prio) == -1)
    {
      perror("Failed serviceServerQueue receive");
      exit(1);
    }
    if(prio != DIED_PRIO)
    {
      addClient(clientName);
      broadcastNicknames();
    }
    else
    {
      cleanClients();
      broadcastNicknames();
    }
  }

  if(mq_close(serviceServerQueue) == -1)
  {
    perror("Failed close");
    exit(1);
  }
  if(mq_unlink("/serviceServerQueue") == -1)
  {
    perror("Failed unlink");
    exit(1);
  }
}
void broadcastMsgs(char **msgs)
{
  char msgQueueName[MAX_LENGTH_NICKNAME+4];
  for(int i = 0; i<countClients; i++)
  {
    snprintf(msgQueueName, sizeof(msgQueueName), "/msg%s", clients[i].nickname);
    mqd_t msgQueue = mq_open(msgQueueName, O_WRONLY);
    if (msgQueue == -1)
    {
      /* perror("Failed open msg queue"); */
      /* exit(1); */
      /* clients[i].isActive = 0; */
    }

      printf("count MSGS - %d\n", countMsgs);
    mq_send(msgQueue, "\0", 2, CLEAR_PRIO);
    for(int j = 0; j<countMsgs; j++)
    {
      mq_send(msgQueue, msgs[j], strlen(msgs[j]) + 1, NICKNAME_PRIO);
      printf("msg  -  %s\n", msgs[j]);
    }
    mq_close(msgQueue);
    printf("%s\n", msgs[i]);
  }
  printf("------\n");
}
void *msgMain(void* args)
{

  struct mq_attr attr;
  attr.mq_maxmsg = MAX_COUNT_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;

  mqd_t msgServerQueue = mq_open("/msgSndServerQueue", O_RDONLY | O_CREAT, 0600, &attr);
  if (msgServerQueue == -1)
  {
    perror("Failed create queue");
    exit(EXIT_FAILURE);
  }

  char **msgs = malloc(sizeof(char*)*MAX_COUNT_MSGS);
  for(int i = 0; i < MAX_COUNT_MSGS; i++)
  {
    msgs[i] = malloc(sizeof(char)*MAX_MSG_SIZE);
  }

  while(1)
  {
    if(mq_receive(msgServerQueue, msgs[countMsgs], MAX_MSG_SIZE, NULL) == -1)
    {
      perror("Failed msgServerQueue receive");
      exit(1);
    }
    countMsgs++;

    broadcastMsgs(msgs);
  }

  for(int i = 0; i < MAX_COUNT_MSGS; i++)
  {
    free(msgs[i]);
  }
  free(msgs);

  if(mq_close(msgServerQueue) == -1)
  {
    perror("Failed close");
    exit(1);
  }
  if(mq_unlink("/msgSndServerQueue") == -1)
  {
    perror("Failed unlink");
    exit(1);
  }
}
int main()
{
  pthread_t nicknameThread;
  pthread_t msgThread;

  pthread_create(&nicknameThread, NULL, nicknameMain, NULL);
  pthread_create(&msgThread, NULL, msgMain, NULL);

  pthread_join(nicknameThread,NULL);
  pthread_join(msgThread,NULL);

  return 0;
}
