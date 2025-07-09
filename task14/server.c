#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#define MAX_COUNT_MSGS 50
#define MAX_LENGTH_MSG 256

#define MAX_COUNT_NICKNAMES 16
#define MAX_LENGTH_NICKNAME 20

char *nicknames; 
char *nickname; 
char **msgs;
int *countClients;
int *isUsedNickname;

int countMsgs = 0;

int shmMsg;
int shmNicknames;
int shmNickname;
int shmCountClients;
int shmIsUsedNickname;

sem_t *semLockMsg;
sem_t *semLockNicknames;
sem_t *semLockNickname;

sem_t *semWaitMsg;
sem_t *semWaitUpdateNickname; //Ожидание пока все клиенты обновят информацию
sem_t *semWaitOtherClients;
sem_t *semWaitNickname;
sem_t *semWaitBroadcast;

void cleanAll()
{
  //TODO unmap

  close(shmMsg);
  shm_unlink("/shmMsg");
  close(shmNicknames);
  shm_unlink("/shmNicknames");
  close(shmNickname);
  shm_unlink("/shmNickname");
  close(shmIsUsedNickname);
  shm_unlink("/shmIsUsedNickname");
  close(shmCountClients);
  shm_unlink("/shmCountClients");

  sem_close(semLockMsg);
  sem_unlink("/semLockMsg");
  sem_close(semLockNicknames);
  sem_unlink("/semLockNicknames");
  sem_close(semLockNickname);
  sem_unlink("/semLockNickname");
  sem_close(semWaitMsg);
  sem_unlink("/semWaitMsg");
  sem_close(semWaitUpdateNickname);
  sem_unlink("/semWaitUpdateNickname");
  sem_close(semWaitOtherClients);
  sem_unlink("/semWaitOtherClients");
  sem_close(semWaitNickname);
  sem_unlink("/semWaitNickname");
  sem_close(semWaitBroadcast);
  sem_unlink("/semWaitBroadcast");
}
/* void cleanClients() */
/* { */
/*   int i =0; */
/*   char bufNick[MAX_LENGTH_NICKNAME+1]; */
/*   for(int j = 0; j<countClients; j++) */
/*   { */
/*     snprintf(bufNick, sizeof(bufNick), "/%s", clients[j].nickname); */
/*     mqd_t client = mq_open(bufNick, O_WRONLY); */
/*     if (client == -1) */
/*     { */
/*       printf("client %s - DIE\n", clients[j].nickname); */
/*       clients[j].isActive = 0; */
/*     } */
/*     else */
/*     { */
/*       mq_close(client); */
/*     } */
/*   } */
/*   while(i<countClients) */
/*   { */
/*     if(!clients[i].isActive) */
/*     { */
/*       for(int j =i; j<countClients-1; j++) */
/*       { */
/*         clients[j] = clients[j+1]; */
/*       } */
/*       countClients--; */
/*     } */
/*     else */
/*     { */
/*       i++; */
/*     } */
/*   } */
/* } */
void addClient(char *nickname)
{
  if(*countClients >= MAX_COUNT_NICKNAMES)
  {
    perror("Failed - max count clients");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  printf("%s\n", nickname);
  strncpy(nicknames+(*countClients * MAX_LENGTH_NICKNAME), nickname, MAX_LENGTH_NICKNAME);
  printf("nicknames[%d] = %s\n", *countClients ,nicknames+(*countClients * MAX_LENGTH_NICKNAME));
  (*countClients)++;
}
/* void broadcastNicknames() */
/* { */
/*   printf("COUNT CLIENTS %d\n", countClients); */
/*   char bufNick[MAX_LENGTH_NICKNAME+1]; */
/*   for(int i = 0; i<countClients; i++) */
/*   { */
/*     snprintf(bufNick, sizeof(bufNick), "/%s", clients[i].nickname); */
/*     mqd_t client = mq_open(bufNick, O_WRONLY); */
/*     if (client == -1) */
/*     { */
/*       clients[i].isActive = 0; */
/*     } */

/*     mq_send(client, "\0", 2, CLEAR_PRIO); */
/*     for(int j = 0; j<countClients; j++) */
/*     { */
/*       mq_send(client, clients[j].nickname, strlen(clients[j].nickname) + 1, NICKNAME_SET_PRIO); */
/*       printf("%s\n", clients[j].nickname); */
/*     } */
/*     mq_close(client); */

/*     printf("%s\n", bufNick); */
/*   } */
/*   printf("------\n"); */
/* } */
void *nicknameMain(void* args)
{
  shmNicknames = shm_open("/shmNicknames", O_CREAT|O_RDWR, 0600);
  if (shmNicknames == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  ftruncate(shmNicknames, MAX_LENGTH_NICKNAME*MAX_COUNT_NICKNAMES);
  nicknames = mmap(NULL, MAX_LENGTH_NICKNAME*MAX_COUNT_NICKNAMES, PROT_READ|PROT_WRITE, MAP_SHARED, shmNicknames, 0);
  if (nicknames == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  shmNickname = shm_open("/shmNickname", O_CREAT|O_RDWR, 0600);
  if (shmNickname == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  ftruncate(shmNickname, MAX_LENGTH_NICKNAME);
  nickname = mmap(NULL, MAX_LENGTH_NICKNAME, PROT_READ|PROT_WRITE, MAP_SHARED, shmNickname, 0);
  if (nickname == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  /* memset(nickname, 0, MAX_LENGTH_NICKNAME); */

  shmCountClients = shm_open("/shmCountClients", O_CREAT|O_RDWR, 0600);
  if (shmCountClients == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  ftruncate(shmCountClients, sizeof(int));
  countClients = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shmCountClients, 0);
  if (countClients == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  *countClients = 0;

  shmIsUsedNickname = shm_open("/shmIsUsedNickname", O_CREAT|O_RDWR, 0600);
  if (shmIsUsedNickname == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  ftruncate(shmIsUsedNickname, sizeof(int));
  isUsedNickname = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shmIsUsedNickname, 0);
  if (isUsedNickname == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  /* semaphores */
  semLockNicknames = sem_open("/semLockNicknames", O_CREAT|O_RDWR, 0600, 1);
  if (semLockNicknames == SEM_FAILED)
  {
    perror("Failed create lock semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semLockNickname = sem_open("/semLockNickname", O_CREAT|O_RDWR, 0600, 1);
  if (semLockNicknames == SEM_FAILED)
  {
    perror("Failed create lock semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitUpdateNickname = sem_open("/semWaitUpdateNickname", O_CREAT|O_RDWR, 0600, 0);
  if (semWaitUpdateNickname == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitOtherClients = sem_open("/semWaitOtherClients", O_CREAT|O_RDWR, 0600, 0);
  if (semWaitOtherClients == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitNickname = sem_open("/semWaitNickname", O_CREAT|O_RDWR, 0600, 0);
  if (semWaitNickname == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitBroadcast = sem_open("/semWaitBroadcast", O_CREAT|O_RDWR, 0600, 0);
  if (semWaitBroadcast == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  int curCount = 0;
  for(;;)
  {
    sem_wait(semWaitNickname); //ждем пока поступят сообщения

    sem_wait(semLockNicknames);
    /* cleanClients(); */
    /* checkNickname(); */
    addClient(nickname);
    curCount = *countClients;
    sem_post(semLockNicknames);

    printf("%d\n", curCount);
    for(int i = 0; i < curCount; i++)
    {
      sem_post(semWaitBroadcast); //разблокируем семафоры на стороне клиентов
    }
    for(int i = 0; i < curCount; i++)
    {
      sem_wait(semWaitUpdateNickname);
    }
    for(int i = 0; i < curCount; i++)
    {
      sem_post(semWaitOtherClients);
    }
  }
  
  /* --------------------------- */
  /* serviceServerQueue = mq_open("/serviceServerQueue", O_RDWR | O_CREAT, 0600, &attr); */
  /* if (serviceServerQueue == -1) */
  /* { */
  /*   perror("Failed create queue"); */
  /*   cleanAll(); */
  /*   exit(EXIT_FAILURE); */
  /* } */

  /* char clientName[MAX_LENGTH_NICKNAME]; */
  /* unsigned int prio; */
  /* int isAlreadyUsed = 0; */
  /* while(1) */
  /* { */
  /*   if(mq_receive(serviceServerQueue, clientName, MAX_LENGTH_NICKNAME, &prio) == -1) */
  /*   { */
  /*     perror("Failed serviceServerQueue receive"); */
  /*     cleanAll(); */
  /*     exit(EXIT_FAILURE); */
  /*   } */
  /*   if(prio == NICKNAME_CHECK_PRIO) */
  /*   { */
  /*     for(int i = 0; i < countClients; i++) */
  /*     { */
  /*       if(strcmp(clientName,clients[i].nickname) == 0) */
  /*       { */
  /*         isAlreadyUsed = 1; */
  /*         break; */
  /*       } */
  /*     } */
  /*     if(isAlreadyUsed) */
  /*     { */
  /*       if (mq_send(serviceServerQueue, "\0", 2, NICKNAME_NO_CONFIRM_PRIO) == -1) */
  /*       { */
  /*         perror("Failed send"); */
  /*         cleanAll(); */
  /*         exit(EXIT_FAILURE); */
  /*       } */
  /*       isAlreadyUsed = 0; */
  /*     } */
  /*     else */
  /*     { */
  /*       if (mq_send(serviceServerQueue, "\0", 2, NICKNAME_CONFIRM_PRIO) == -1) */
  /*       { */
  /*         perror("Failed send"); */
  /*         cleanAll(); */
  /*         exit(EXIT_FAILURE); */
  /*       } */
  /*     } */
  /*   } */
  /*   else if (prio == NICKNAME_SET_PRIO) */
  /*   { */
  /*     addClient(clientName); */
  /*     broadcastNicknames(); */
  /*   } */
  /*   else if(prio == DIED_PRIO) */
  /*   { */
  /*     cleanClients(); */
  /*     broadcastNicknames(); */
  /*   } */
  /* } */

}
/* void broadcastMsgs(char **msgs) */
/* { */
/*   char msgQueueName[MAX_LENGTH_NICKNAME+4]; */
/*   for(int i = 0; i<countClients; i++) */
/*   { */
/*     snprintf(msgQueueName, sizeof(msgQueueName), "/msg%s", clients[i].nickname); */
/*     mqd_t msgQueue = mq_open(msgQueueName, O_WRONLY); */
/*     if (msgQueue == -1) */
/*     { */
/*       clients[i].isActive = 0; */
/*     } */

/*     printf("count MSGS - %d\n", countMsgs); */
/*     mq_send(msgQueue, "\0", 2, CLEAR_PRIO); */
/*     for(int j = 0; j<countMsgs; j++) */
/*     { */
/*       mq_send(msgQueue, msgs[j], strlen(msgs[j]) + 1, MSG_PRIO); */
/*       printf("msg  -  %s\n", msgs[j]); */
/*     } */
/*     mq_close(msgQueue); */
/*   } */
/*   printf("------\n"); */
/* } */
/* void *msgMain(void* args) */
/* { */
/*   struct mq_attr attr; */
/*   attr.mq_maxmsg = MAX_COUNT_MSGS_IN_QUEUE; */
/*   attr.mq_msgsize = MAX_LENGTH_MSG; */

/*   msgServerQueue = mq_open("/msgSndServerQueue", O_RDONLY | O_CREAT, 0600, &attr); */
/*   if (msgServerQueue == -1) */
/*   { */
/*     perror("Failed create queue"); */
/*     cleanAll(); */
/*     exit(EXIT_FAILURE); */
/*   } */

/*   msgs = malloc(sizeof(char*)*MAX_COUNT_MSGS); */
/*   for(int i = 0; i < MAX_COUNT_MSGS; i++) */
/*   { */
/*     msgs[i] = malloc(sizeof(char)*MAX_LENGTH_MSG); */
/*   } */

/*   while(1) */
/*   { */
/*     if(mq_receive(msgServerQueue, msgs[countMsgs], MAX_LENGTH_MSG, NULL) == -1) */
/*     { */
/*       perror("Failed msgServerQueue receive"); */
/*       cleanAll(); */
/*       exit(EXIT_FAILURE); */
/*     } */
/*     countMsgs++; */
/*     if(countMsgs >= MAX_COUNT_MSGS) */
/*     { */
/*       fprintf(stderr, "Limit messages\n"); */
/*       cleanAll(); */
/*       exit(EXIT_FAILURE); */
/*     } */
/*     broadcastMsgs(msgs); */
/*   } */

/* } */
int main()
{
  pthread_t nicknameThread;
  /* pthread_t msgThread; */

  pthread_create(&nicknameThread, NULL, nicknameMain, NULL);
  /* pthread_create(&msgThread, NULL, msgMain, NULL); */

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
  /* pthread_cancel(msgThread); */

  pthread_join(nicknameThread, NULL);
  /* pthread_join(msgThread, NULL); */

  cleanAll();

  return 0;
}
