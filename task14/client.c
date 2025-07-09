#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "view.h"

#define MAX_COUNT_MSGS 50
#define MAX_LENGTH_MSG 256

#define MAX_COUNT_NICKNAMES 16
#define MAX_LENGTH_NICKNAME 20

char *nicknames; 
char **msgs;
char *nickname;
int *countClients;
int *isUsedNickname;

Chat* chat;

int shmMsg;
int shmNicknames;
int shmNickname;
int shmCountClients;
int shmIsUsedNickname;

sem_t *semLockMsg;
sem_t *semLockNicknames;
sem_t *semLockNickname;
sem_t *semWaitMsg;
sem_t *semWaitNicknames;
sem_t *semWaitNickname;
sem_t *semWaitBroadcast;

void cleanAll()
{
  munmap(nicknames, MAX_LENGTH_NICKNAME*MAX_COUNT_NICKNAMES);
  munmap(nickname, MAX_LENGTH_NICKNAME);
  munmap(msgs, MAX_LENGTH_MSG);
  munmap(countClients, sizeof(int));
  munmap(isUsedNickname, sizeof(int));

  close(shmMsg);
  close(shmNicknames);
  close(shmNickname);

  sem_close(semLockMsg);
  sem_close(semLockNicknames);
  sem_close(semLockNickname);
  sem_close(semWaitMsg);
  sem_close(semWaitNicknames);
  sem_close(semWaitNickname);
  sem_close(semWaitBroadcast);
}
void* receiveNicknames(void *args)
{
  int i;
  for(;;)
  {
    i = 0;
    sem_wait(semWaitBroadcast);

    sem_wait(semLockNicknames);
    /* while(nickname[i] != NULL) */
    /* { */
      printNickname(chat, nicknames, nickname);
    /*   i++; */
    /* } */
    sem_post(semLockNicknames);
  }
/* ----------------------- */
  /* mqd_t* receiveQueue = (mqd_t*)receiveQueueVoid; */
  /* char clientName[MAX_LENGTH_NICKNAME]; */
  /* unsigned int prio = 0; */
  /* while (1) */
  /* { */
  /*   if (mq_receive(*receiveQueue, clientName, MAX_LENGTH_NICKNAME, &prio) == -1) */
  /*   { */
  /*     perror("Failed receive"); */
  /*     cleanAll(); */
  /*     exit(EXIT_FAILURE); */
  /*   } */
  /*   if (prio == NICKNAME_SET_PRIO) */
  /*   { */
  /*     printNickname(chat, clientName, nickname); */
  /*   } */
  /*   else if (prio == CLEAR_PRIO) */
  /*   { */
  /*     clearNicknameWin(chat); */
  /*   } */
  /* } */
}
/* void* receiveMsgs(void* args) */
/* { */
/*   mqd_t* receiveQueue = (mqd_t*)receiveQueueVoid; */
/*   char msg[MAX_LENGTH_MSG]; */
/*   unsigned int prio = 0; */
/*   while (1) */
/*   { */
/*     if (mq_receive(*receiveQueue, msg, MAX_LENGTH_MSG, &prio) == -1) */
/*     { */
/*       perror("Failed receive"); */
/*       cleanAll(); */
/*       exit(EXIT_FAILURE); */
/*     } */
/*     if (prio == MSG_PRIO) */
/*     { */
/*       printMsg(chat, msg); */
/*     } */
/*     else if (prio == CLEAR_PRIO) */
/*     { */
/*       clearMsgWin(chat); */
/*     } */
/*   } */
/* } */
/* void sendMsgInChat(mqd_t* msgSndServerQueue) */
/* { */
/*   char msg[MAX_LENGTH_MSG]; */
/*   char msgWithNickname[MAX_LENGTH_MSG]; */
/*   while (1) */
/*   { */
/*     enterMsg(chat, msg, MAX_LENGTH_MSG); */
/*     if (strcmp(msg, "exit") == 0) */
/*     { */
/*       return; */
/*     } */
/*     snprintf(msgWithNickname, MAX_LENGTH_MSG, "%s: %s",nickname ,msg); */
/*     if (mq_send(*msgSndServerQueue, msgWithNickname, strlen(msgWithNickname)+1, MSG_PRIO) == -1) */
/*     { */
/*       perror("Failed send"); */
/*       cleanAll(); */
/*       exit(EXIT_FAILURE); */
/*     } */
/*   } */
/* } */
void enterNickname()
{
    printf("Enter nickname\n");
    /* sem_wait(semLockNickname); */
    scanf("%19s", nickname);
    /* fgets(nickname, MAX_LENGTH_NICKNAME, stdin); */
    /* sem_post(semLockNickname); */
    sem_post(semWaitNickname);

    /* ------------------ */
  /* char tempBuf[MAX_LENGTH_NICKNAME]; */
  /* unsigned int prio = NICKNAME_NO_CONFIRM_PRIO; */
  /* while(prio == NICKNAME_NO_CONFIRM_PRIO) */
  /* { */
  /*   printf("Enter nickname\n"); */
  /*   scanf("%19s", nickname); */
  /*   if (mq_send(serviceServerQueue, nickname, strlen(nickname) + 1, NICKNAME_CHECK_PRIO) == -1) */
  /*   { */
  /*     perror("Failed send"); */
  /*     cleanAll(); */
  /*     exit(EXIT_FAILURE); */
  /*   } */

  /*   if(mq_receive(serviceServerQueue, tempBuf, MAX_LENGTH_NICKNAME, &prio) == -1) */
  /*   { */
  /*     perror("Failed serviceServerQueue receive"); */
  /*     cleanAll(); */
  /*     exit(EXIT_FAILURE); */
  /*   } */
  /*   if(prio == NICKNAME_NO_CONFIRM_PRIO) */
  /*   { */
  /*     printf("NICKNAME ALREADY USED\n"); */
  /*   } */
  /* } */
}

void initShmNicknames()
{
  shmNicknames = shm_open("/shmNicknames", O_RDWR, 0);
  if (shmNicknames == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  nicknames = mmap(NULL, MAX_LENGTH_NICKNAME*MAX_COUNT_NICKNAMES, PROT_READ|PROT_WRITE, MAP_SHARED, shmNicknames, 0);

  int shmNickname = shm_open("/shmNickname", O_RDWR, 0);
  if (shmNickname == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  nickname = mmap(NULL, MAX_LENGTH_NICKNAME, PROT_READ|PROT_WRITE, MAP_SHARED, shmNickname, 0);
  if (nickname == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  shmCountClients = shm_open("/shmCountClients", O_RDWR, 0);
  if (shmCountClients == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  countClients = mmap(NULL, MAX_LENGTH_NICKNAME, PROT_READ|PROT_WRITE, MAP_SHARED, shmCountClients, 0);
  if (countClients == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  shmIsUsedNickname = shm_open("/shmIsUsedNickname", O_RDWR, 0);
  if (shmIsUsedNickname == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  isUsedNickname = mmap(NULL, MAX_LENGTH_NICKNAME, PROT_READ|PROT_WRITE, MAP_SHARED, shmIsUsedNickname, 0);
  if (isUsedNickname == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  semLockNicknames = sem_open("/semLockNicknames", O_RDWR);
  if (semLockNicknames == SEM_FAILED)
  {
    perror("Failed create lock semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semLockNickname = sem_open("/semLockNickname", O_RDWR);
  if (semLockNickname == SEM_FAILED)
  {
    perror("Failed create lock semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitNicknames = sem_open("/semWaitNicknames", O_RDWR);
  if (semWaitNicknames == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitNickname = sem_open("/semWaitNickname", O_RDWR);
  if (semWaitNickname == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitBroadcast = sem_open("/semWaitBroadcast", O_RDWR);
  if (semWaitNicknames == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  /* --------------------- */
  /* struct mq_attr attrNickname; */
  /* attrNickname.mq_maxmsg = MAX_COUNT_MSGS_IN_QUEUE; */
  /* attrNickname.mq_msgsize = MAX_LENGTH_NICKNAME; */

  /* serviceServerQueue = mq_open("/serviceServerQueue", O_RDWR); */
  /* if (serviceServerQueue == -1) */
  /* { */
  /*   perror("Failed open service queue"); */
  /*   cleanAll(); */
  /*   exit(EXIT_FAILURE); */
  /* } */
  /* enterNickname(); */

  /* strcat(bufNick, nickname); */
  /* serviceClientQueue = mq_open(bufNick, O_RDONLY | O_CREAT, 0600, &attrNickname); */
  /* if (serviceClientQueue == -1) */
  /* { */
  /*   perror("Failed create service queue"); */
  /*   cleanAll(); */
  /*   exit(EXIT_FAILURE); */
  /* } */
}
/* void openMsgQueue() */
/* { */
/*   struct mq_attr attrMsg; */
/*   attrMsg.mq_maxmsg = MAX_COUNT_MSGS_IN_QUEUE; */
/*   attrMsg.mq_msgsize = MAX_LENGTH_MSG; */

/*   msgSndServerQueue = mq_open("/msgSndServerQueue", O_WRONLY); */
/*   if (msgSndServerQueue == -1) */
/*   { */
/*     perror("Failed open msg queue"); */
/*     cleanAll(); */
/*     exit(EXIT_FAILURE); */
/*   } */

/*   snprintf(msgQueueName, MAX_LENGTH_NICKNAME + 4, "/msg%s", nickname); */
/*   msgClientQueue = mq_open(msgQueueName, O_RDONLY | O_CREAT, 0600, &attrMsg); */
/*   if (msgClientQueue == -1) */
/*   { */
/*     perror("Failed create msg queue"); */
/*     cleanAll(); */
/*     exit(EXIT_FAILURE); */
/*   } */
/* } */
int main()
{
  initShmNicknames();
  enterNickname();
  /* openMsgQueue(); */

  initChat();
  chat = createChat();
  refreshChat(chat);
  getchar();


  pthread_t nicknameThread;
  /* pthread_t msgThread; */

  pthread_create(&nicknameThread, NULL, receiveNicknames, NULL);
  /* pthread_create(&msgThread, NULL, receiveMsgs, (void*)&msgClientQueue); */

  /* if (mq_send(serviceServerQueue, nickname, strlen(nickname) + 1, NICKNAME_SET_PRIO) == -1) */
  /* { */
  /*   perror("Failed send"); */
  /*   cleanAll(); */
  /*   exit(EXIT_FAILURE); */
  /* } */

  for(;;);

  /* sendMsgInChat(&msgSndServerQueue); */

  pthread_cancel(nicknameThread);
  /* pthread_cancel(msgThread); */

  pthread_join(nicknameThread,NULL);
  /* pthread_join(msgThread, NULL); */

  destroyChat(chat);
  cleanAll();

  return 0;
}
