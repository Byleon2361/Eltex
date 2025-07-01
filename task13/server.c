#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define MAX_COUNT_MESSAGES 8
#define MAX_MSG_SIZE 20

#define MSG_PRIO 1
#define NICKNAME_PRIO 2
#define MAX_LENGTH_MSG 256
#define MAX_LENGTH_NICKNAME 20
#define MAX_COUNT_MSGS 50
#define MAX_COUNT_NICKNAMES 16

typedef struct client
{
  char nickname[MAX_LENGTH_NICKNAME];
  int isActive;
} Client; 
Client clients[MAX_COUNT_NICKNAMES]; 
int countClients = 0;

void cleanClients()
{
  int i =0;
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
int main()
{
  char msgs[MAX_COUNT_MSGS][MAX_LENGTH_MSG];

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
  while(1)
  {
    if(mq_receive(serviceServerQueue, clientName, sizeof(clientName), NULL) == -1)
    {
      perror("Failed serviceServerQueue receive");
      exit(1);
    }

    cleanClients();
    addClient(clientName);

    int status =0;
  char bufNick[MAX_LENGTH_NICKNAME+1];
    for(int i = 0; i<countClients; i++)
    {
  snprintf(bufNick, sizeof(bufNick), "/%s", clients[i].nickname);

  mqd_t client = mq_open(bufNick, O_WRONLY);
      status = mq_send(client, clients[i].nickname, strlen(clients[i].nickname) + 1, NICKNAME_PRIO);
  mq_close(client);
      if(status == -1)
      {
        clients[i].isActive = 0;
      }

      printf("%s", clients[i].nickname);
    }
      printf("------\n");
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

  return 0;
}
