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

/* SHARED DATA */
char *nicknames; 
char *nickname; 
int *countClients;
int *isUsedNickname;

char *msgs;
int *countMsgs;

/* SHARED MEMORY ID*/
int shmNicknames;
int shmNickname;
int shmCountClients;
int shmIsUsedNickname;

int shmMsgs;
int shmCountMsgs;

/* LOCK */
sem_t *semLockNicknames;
sem_t *semLockNickname;

sem_t *semLockMsgs;
sem_t *semLockMsg;

/* WAIT */
sem_t *semWaitUpdateNickname; //Сервер ожиданает пока все клиенты обновят информацию
sem_t *semWaitOtherClients; //Клиент ожидает пока все клиенты обновят информацию
sem_t *semWaitNickname; //Ожидание присоединения нового клиента
sem_t *semWaitBroadcastNicknames; //Ожидание broadcast

sem_t *semWaitUpdateMsg; 
sem_t *semWaitOtherMsgs;
sem_t *semWaitMsg;
sem_t *semWaitBroadcastMsgs;

void cleanAll()
{
  munmap(nicknames, MAX_LENGTH_NICKNAME*MAX_COUNT_NICKNAMES);
  munmap(nickname, MAX_LENGTH_NICKNAME);
  munmap(msgs, MAX_LENGTH_MSG);
  munmap(countClients, sizeof(int));
  munmap(isUsedNickname, sizeof(int));

  munmap(msgs, MAX_LENGTH_MSG*MAX_COUNT_MSGS);
  munmap(countMsgs,sizeof(int));

  /* SHM NICKNAMES */
  close(shmNicknames);
  shm_unlink("/shmNicknames");
  close(shmNickname);
  shm_unlink("/shmNickname");
  close(shmIsUsedNickname);
  shm_unlink("/shmIsUsedNickname");
  close(shmCountClients);
  shm_unlink("/shmCountClients");

  /* SHM MSGS */
  close(shmMsgs);
  shm_unlink("/shmMsgs");
  close(shmCountMsgs);
  shm_unlink("/shmCountMsgs");

  /* SEM NICKNAMES */
  sem_close(semLockNicknames);
  sem_unlink("/semLockNicknames");
  sem_close(semLockNickname);
  sem_unlink("/semLockNickname");

  sem_close(semWaitUpdateNickname);
  sem_unlink("/semWaitUpdateNickname");
  sem_close(semWaitOtherClients);
  sem_unlink("/semWaitOtherClients");
  sem_close(semWaitNickname);
  sem_unlink("/semWaitNickname");
  sem_close(semWaitBroadcastNicknames);
  sem_unlink("/semWaitBroadcastNicknames");

  /* SEM MSGS */
  sem_close(semLockMsgs);
  sem_unlink("/semLockMsgs");
  sem_close(semLockMsg);
  sem_unlink("/semLockMsg");

  sem_close(semWaitUpdateMsg);
  sem_unlink("/semWaitUpdateMsg");
  sem_close(semWaitOtherMsgs);
  sem_unlink("/semWaitOtherMsgs");
  sem_close(semWaitMsg);
  sem_unlink("/semWaitMsg");
  sem_close(semWaitBroadcastMsgs);
  sem_unlink("/semWaitBroadcastMsgs");
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
  semWaitBroadcastNicknames = sem_open("/semWaitBroadcastNicknames", O_CREAT|O_RDWR, 0600, 0);
  if (semWaitBroadcastNicknames == SEM_FAILED)
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

    printf("Count clients %d\n", curCount);
    for(int i = 0; i < curCount; i++)
    {
      sem_post(semWaitBroadcastNicknames); //разблокируем семафоры на стороне клиентов
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
}
void *msgMain(void* args)
{
  shmMsgs = shm_open("/shmMsgs", O_CREAT|O_RDWR, 0600);
  if (shmMsgs == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  ftruncate(shmMsgs, MAX_LENGTH_MSG*MAX_COUNT_MSGS);
  msgs = mmap(NULL, MAX_LENGTH_MSG*MAX_COUNT_MSGS, PROT_READ|PROT_WRITE, MAP_SHARED, shmMsgs, 0);
  if (msgs == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  shmCountMsgs = shm_open("/shmCountMsgs", O_CREAT|O_RDWR, 0600);
  if (shmCountMsgs == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  ftruncate(shmCountMsgs, sizeof(int));
  countMsgs = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shmCountMsgs, 0);
  if (countMsgs == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  *countMsgs = 0;

  /* semaphores */
  semLockMsgs = sem_open("/semLockMsgs", O_CREAT|O_RDWR, 0600, 1);
  if (semLockMsgs == SEM_FAILED)
  {
    perror("Failed create lock semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semLockMsg = sem_open("/semLockMsg", O_CREAT|O_RDWR, 0600, 1);
  if (semLockMsg == SEM_FAILED)
  {
    perror("Failed create lock semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitUpdateMsg = sem_open("/semWaitUpdateMsg", O_CREAT|O_RDWR, 0600, 0);
  if (semWaitUpdateMsg == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitOtherMsgs = sem_open("/semWaitOtherMsgs", O_CREAT|O_RDWR, 0600, 0);
  if (semWaitOtherMsgs == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitMsg = sem_open("/semWaitMsg", O_CREAT|O_RDWR, 0600, 0);
  if (semWaitMsg == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitBroadcastMsgs = sem_open("/semWaitBroadcastMsgs", O_CREAT|O_RDWR, 0600, 0);
  if (semWaitBroadcastMsgs == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }

  int curCountClients = 0;
  for(;;)
  {
    sem_wait(semWaitMsg); //ждем пока поступят сообщения

    (*countMsgs)++;

    sem_wait(semLockNicknames);
    curCountClients = *countClients;
    sem_post(semLockNicknames);

    sem_wait(semLockMsgs);
    printf("Count msgs %d\n", *countMsgs);
    printf("msg %s\n", msgs+((*countMsgs-1)*MAX_LENGTH_MSG));
    sem_post(semLockMsgs);

    for(int i = 0; i < curCountClients; i++)
    {
      sem_post(semWaitBroadcastMsgs); //разблокируем семафоры на стороне клиентов
    }
    for(int i = 0; i < curCountClients; i++)
    {
      sem_wait(semWaitUpdateMsg);
    }
    for(int i = 0; i < curCountClients; i++)
    {
      sem_post(semWaitOtherMsgs);
    }
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
