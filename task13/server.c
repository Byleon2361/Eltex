#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define NICKNAME_SET_PRIO 2
#define CLEAR_PRIO 3
#define DIED_PRIO 4
#define MSG_PRIO 1
#define NICKNAME_NO_CONFIRM_PRIO 5
#define NICKNAME_CONFIRM_PRIO 6
#define NICKNAME_CHECK_PRIO 7

#define MAX_COUNT_MSGS_IN_QUEUE 10

#define MAX_COUNT_MSGS 50
#define MAX_LENGTH_MSG 256

#define MAX_COUNT_NICKNAMES 16
#define MAX_LENGTH_NICKNAME 20

typedef struct client
{
  char nickname[MAX_LENGTH_NICKNAME];
  int isActive;
} Client; 
Client clients[MAX_COUNT_NICKNAMES]; 

int countClients = 0;
int countMsgs = 0;

char **msgs;
mqd_t msgServerQueue = 0;
mqd_t serviceServerQueue = 0;

void cleanAll()
{
  for(int i = 0; i < MAX_COUNT_MSGS; i++)
  {
    free(msgs[i]);
  }
  free(msgs);

  if(mq_close(msgServerQueue) == -1)
  {
    perror("Failed close");
  }
  if(mq_unlink("/msgSndServerQueue") == -1)
  {
    perror("Failed unlink");
  }
  if(mq_close(serviceServerQueue) == -1)
  {
    perror("Failed close");
  }
  if(mq_unlink("/serviceServerQueue") == -1)
  {
    perror("Failed unlink");
  }
}
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
    cleanAll();
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
      mq_send(client, clients[j].nickname, strlen(clients[j].nickname) + 1, NICKNAME_SET_PRIO);
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
  attr.mq_maxmsg = MAX_COUNT_MSGS_IN_QUEUE;
  attr.mq_msgsize = MAX_LENGTH_NICKNAME;

  serviceServerQueue = mq_open("/serviceServerQueue", O_RDWR | O_CREAT, 0600, &attr);
  if (serviceServerQueue == -1)
  {
    perror("Failed create queue");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  char clientName[MAX_LENGTH_NICKNAME];
  unsigned int prio;
  int isAlreadyUsed = 0;
  while(1)
  {
    if(mq_receive(serviceServerQueue, clientName, MAX_LENGTH_NICKNAME, &prio) == -1)
    {
      perror("Failed serviceServerQueue receive");
      cleanAll();
      exit(EXIT_FAILURE);
    }
    if(prio == NICKNAME_CHECK_PRIO)
    {
      for(int i = 0; i < countClients; i++)
      {
        if(strcmp(clientName,clients[i].nickname) == 0)
        {
          isAlreadyUsed = 1;
          break;
        }
      }
      if(isAlreadyUsed)
      {
        if (mq_send(serviceServerQueue, "\0", 2, NICKNAME_NO_CONFIRM_PRIO) == -1)
        {
          perror("Failed send");
          cleanAll();
          exit(EXIT_FAILURE);
        }
        isAlreadyUsed = 0;
      }
      else
      {
        if (mq_send(serviceServerQueue, "\0", 2, NICKNAME_CONFIRM_PRIO) == -1)
        {
          perror("Failed send");
          cleanAll();
          exit(EXIT_FAILURE);
        }
      }
    }
    else if (prio == NICKNAME_SET_PRIO)
    {
      addClient(clientName);
      broadcastNicknames();
    }
    else if(prio == DIED_PRIO)
    {
      cleanClients();
      broadcastNicknames();
    }
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
      clients[i].isActive = 0;
    }

    printf("count MSGS - %d\n", countMsgs);
    mq_send(msgQueue, "\0", 2, CLEAR_PRIO);
    for(int j = 0; j<countMsgs; j++)
    {
      mq_send(msgQueue, msgs[j], strlen(msgs[j]) + 1, MSG_PRIO);
      printf("msg  -  %s\n", msgs[j]);
    }
    mq_close(msgQueue);
  }
  printf("------\n");
}
void *msgMain(void* args)
{

  struct mq_attr attr;
  attr.mq_maxmsg = MAX_COUNT_MSGS_IN_QUEUE;
  attr.mq_msgsize = MAX_LENGTH_MSG;

  msgServerQueue = mq_open("/msgSndServerQueue", O_RDONLY | O_CREAT, 0600, &attr);
  if (msgServerQueue == -1)
  {
    perror("Failed create queue");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  msgs = malloc(sizeof(char*)*MAX_COUNT_MSGS);
  for(int i = 0; i < MAX_COUNT_MSGS; i++)
  {
    msgs[i] = malloc(sizeof(char)*MAX_LENGTH_MSG);
  }

  while(1)
  {
    if(mq_receive(msgServerQueue, msgs[countMsgs], MAX_LENGTH_MSG, NULL) == -1)
    {
      perror("Failed msgServerQueue receive");
      cleanAll();
      exit(EXIT_FAILURE);
    }
    countMsgs++;
    if(countMsgs >= MAX_COUNT_MSGS)
    {
      fprintf(stderr, "Limit messages\n");
      cleanAll();
      exit(EXIT_FAILURE);
    }
    broadcastMsgs(msgs);
  }

}
int main()
{
  pthread_t nicknameThread;
  pthread_t msgThread;

  pthread_create(&nicknameThread, NULL, nicknameMain, NULL);
  pthread_create(&msgThread, NULL, msgMain, NULL);

  char buf[20];
  while(1)
  {
    fgets(buf, 20, stdin);
    if(strcmp(buf, "exit\n") == 0)
    {
      break;
    }
  }

  pthread_cancel(nicknameThread);
  pthread_cancel(msgThread);

  pthread_join(nicknameThread, NULL);
  pthread_join(msgThread, NULL);

  cleanAll();

  return 0;
}
