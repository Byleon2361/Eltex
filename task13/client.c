#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>

#include "view.h"

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

Chat* chat;
char nickname[MAX_LENGTH_NICKNAME];

char bufNick[MAX_LENGTH_NICKNAME + 1] = "/";
char msgQueueName[MAX_LENGTH_NICKNAME + 4] = "/";
mqd_t serviceServerQueue = 0;
mqd_t msgSndServerQueue = 0;
mqd_t serviceClientQueue = 0;
mqd_t msgClientQueue = 0;

void cleanAll()
{
  if (mq_close(serviceClientQueue) == -1)
  {
    perror("Failed close");
  }
  if (mq_close(msgSndServerQueue) == -1)
  {
    perror("Failed close");
  }
  if (mq_close(msgClientQueue) == -1)
  {
    perror("Failed close");
  }
  if (mq_unlink(bufNick) == -1)
  {
    perror("Failed unlink");
  }
  if (mq_unlink(msgQueueName) == -1)
  {
    perror("Failed unlink");
  }

  if (mq_send(serviceServerQueue, "\0", 2, DIED_PRIO) == -1)
  {
    perror("Failed send");
  }
  if (mq_close(serviceServerQueue) == -1)
  {
    perror("Failed close");
  }
}
void* receiveNicknames(void* receiveQueueVoid)
{
  mqd_t* receiveQueue = (mqd_t*)receiveQueueVoid;
  char clientName[MAX_LENGTH_NICKNAME];
  unsigned int prio = 0;
  while (1)
  {
    if (mq_receive(*receiveQueue, clientName, MAX_LENGTH_NICKNAME, &prio) == -1)
    {
      perror("Failed receive");
      cleanAll();
      exit(EXIT_FAILURE);
    }
    if (prio == NICKNAME_SET_PRIO)
    {
      printNickname(chat, clientName, nickname);
    }
    else if (prio == CLEAR_PRIO)
    {
      clearNicknameWin(chat);
    }
  }
}
void* receiveMsgs(void* receiveQueueVoid)
{
  mqd_t* receiveQueue = (mqd_t*)receiveQueueVoid;
  char msg[MAX_LENGTH_MSG];
  unsigned int prio = 0;
  while (1)
  {
    if (mq_receive(*receiveQueue, msg, MAX_LENGTH_MSG, &prio) == -1)
    {
      perror("Failed receive");
      cleanAll();
      exit(EXIT_FAILURE);
    }
    if (prio == MSG_PRIO)
    {
      printMsg(chat, msg);
    }
    else if (prio == CLEAR_PRIO)
    {
      clearMsgWin(chat);
    }
  }
}
void sendMsgInChat(mqd_t* msgSndServerQueue)
{
  char msg[MAX_LENGTH_MSG];
  char msgWithNickname[MAX_LENGTH_MSG];
  while (1)
  {
    enterMsg(chat, msg, MAX_LENGTH_MSG);
    if (strcmp(msg, "exit") == 0)
    {
      return;
    }
    snprintf(msgWithNickname, MAX_LENGTH_MSG, "%s: %s",nickname ,msg);
    if (mq_send(*msgSndServerQueue, msgWithNickname, strlen(msgWithNickname)+1, MSG_PRIO) == -1)
    {
      perror("Failed send");
      cleanAll();
      exit(EXIT_FAILURE);
    }
  }
}
void enterNickname()
{
  char tempBuf[MAX_LENGTH_NICKNAME];
  unsigned int prio = NICKNAME_NO_CONFIRM_PRIO;
  while(prio == NICKNAME_NO_CONFIRM_PRIO)
  {
    printf("Enter nickname\n");
    scanf("%19s", nickname);
    if (mq_send(serviceServerQueue, nickname, strlen(nickname) + 1, NICKNAME_CHECK_PRIO) == -1)
    {
      perror("Failed send");
      cleanAll();
      exit(EXIT_FAILURE);
    }

    if(mq_receive(serviceServerQueue, tempBuf, MAX_LENGTH_NICKNAME, &prio) == -1)
    {
      perror("Failed serviceServerQueue receive");
      cleanAll();
      exit(EXIT_FAILURE);
    }
    if(prio == NICKNAME_NO_CONFIRM_PRIO)
    {
      printf("NICKNAME ALREADY USED\n");
    }
  }
}

void openServiceQueue()
{

  struct mq_attr attrNickname;
  attrNickname.mq_maxmsg = MAX_COUNT_MSGS_IN_QUEUE;
  attrNickname.mq_msgsize = MAX_LENGTH_NICKNAME;

  serviceServerQueue = mq_open("/serviceServerQueue", O_RDWR);
  if (serviceServerQueue == -1)
  {
    perror("Failed open service queue");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  enterNickname();

  strcat(bufNick, nickname);
  serviceClientQueue = mq_open(bufNick, O_RDONLY | O_CREAT, 0600, &attrNickname);
  if (serviceClientQueue == -1)
  {
    perror("Failed create service queue");
    cleanAll();
    exit(EXIT_FAILURE);
  }
}
void openMsgQueue()
{
  struct mq_attr attrMsg;
  attrMsg.mq_maxmsg = MAX_COUNT_MSGS_IN_QUEUE;
  attrMsg.mq_msgsize = MAX_LENGTH_MSG;

  msgSndServerQueue = mq_open("/msgSndServerQueue", O_WRONLY);
  if (msgSndServerQueue == -1)
  {
    perror("Failed open msg queue");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  snprintf(msgQueueName, MAX_LENGTH_NICKNAME + 4, "/msg%s", nickname);
  msgClientQueue = mq_open(msgQueueName, O_RDONLY | O_CREAT, 0600, &attrMsg);
  if (msgClientQueue == -1)
  {
    perror("Failed create msg queue");
    cleanAll();
    exit(EXIT_FAILURE);
  }
}
int main()
{
  openServiceQueue();
  openMsgQueue();

  initChat();
  chat = createChat();
  refreshChat(chat);


  pthread_t nicknameThread;
  pthread_t msgThread;

  pthread_create(&nicknameThread, NULL, receiveNicknames, (void*)&serviceClientQueue);
  pthread_create(&msgThread, NULL, receiveMsgs, (void*)&msgClientQueue);

    if (mq_send(serviceServerQueue, nickname, strlen(nickname) + 1, NICKNAME_SET_PRIO) == -1)
    {
      perror("Failed send");
      cleanAll();
      exit(EXIT_FAILURE);
    }
    

  sendMsgInChat(&msgSndServerQueue);

  pthread_cancel(nicknameThread);
  pthread_cancel(msgThread);

  pthread_join(nicknameThread,NULL);
  pthread_join(msgThread, NULL);

  destroyChat(chat);
  cleanAll();

  return 0;
}
