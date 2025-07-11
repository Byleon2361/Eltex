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

Chat* chat;

char curNickname[MAX_LENGTH_NICKNAME]; 
/* SHARED DATA */
char *nicknames; 
int *countClients;
char *nickname; 
int *isUsedNickname;

char *msgs;
int *countMsgs;

/* SHARED MEMORY ID*/
int shmNicknames;
int shmCountClients;
int shmNickname;
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

pthread_mutex_t mutexView = PTHREAD_MUTEX_INITIALIZER;

void cleanAll()
{
  munmap(nicknames, MAX_LENGTH_NICKNAME*MAX_COUNT_NICKNAMES);
  munmap(nickname, MAX_LENGTH_NICKNAME);
  munmap(msgs, MAX_LENGTH_MSG);
  munmap(countClients, sizeof(int));
  munmap(isUsedNickname, sizeof(int));

  munmap(msgs, MAX_LENGTH_MSG*MAX_COUNT_MSGS);
  munmap(countMsgs, sizeof(int));

  /* SHM */
  close(shmMsgs);
  close(shmNicknames);
  close(shmNickname);

  close(shmMsgs);
  close(shmCountMsgs);

  /* SEM */
  sem_close(semLockNicknames);
  sem_close(semLockNickname);
  sem_close(semWaitUpdateNickname);
  sem_close(semWaitOtherClients);
  sem_close(semWaitNickname);
  sem_close(semWaitBroadcastNicknames);

  sem_close(semLockMsgs);
  sem_close(semLockMsg);
  sem_close(semWaitUpdateMsg);
  sem_close(semWaitOtherMsgs);
  sem_close(semWaitMsg);
  sem_close(semWaitBroadcastMsgs);
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

}
void initSemNicknames()
{
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
  semWaitUpdateNickname = sem_open("/semWaitUpdateNickname", O_RDWR);
  if (semWaitUpdateNickname == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitOtherClients = sem_open("/semWaitOtherClients", O_RDWR);
  if (semWaitOtherClients == SEM_FAILED)
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
  semWaitBroadcastNicknames = sem_open("/semWaitBroadcastNicknames", O_RDWR);
  if (semWaitBroadcastNicknames == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
}

void enterNickname()
{
    printf("Enter nickname\n");
    /* sem_wait(semLockNickname); */
    scanf("%19s", curNickname);
    /* strncpy(curNickname, nickname, MAX_LENGTH_NICKNAME); */
    /* sem_post(semLockNickname); */
    /* sem_post(semWaitNickname); */
}
void* receiveNicknames(void *args)
{
  for(;;)
  {
    sem_wait(semWaitBroadcastNicknames);

      pthread_mutex_lock(&mutexView);
    clearNicknameWin(chat);
      pthread_mutex_unlock(&mutexView);
    sem_wait(semLockNicknames);
    for(int i = 0; i < *countClients; i++)
    {
      pthread_mutex_lock(&mutexView);
      printNickname(chat, nicknames+i*MAX_LENGTH_NICKNAME, curNickname);
      pthread_mutex_unlock(&mutexView);
    }
    sem_post(semLockNicknames);

    sem_post(semWaitUpdateNickname); //подтверждаем получение данных
    sem_wait(semWaitOtherClients); //ждем пока осталльные клиенты обновят данные
  }
}
void initShmMsgs()
{
  shmMsgs = shm_open("/shmMsgs", O_RDWR, 0);
  if (shmMsgs == -1)
  {
    perror("Failed create msgs shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  msgs = mmap(NULL, MAX_LENGTH_MSG*MAX_COUNT_MSGS, PROT_READ|PROT_WRITE, MAP_SHARED, shmMsgs, 0);
  shmCountMsgs = shm_open("/shmCountMsgs", O_RDWR, 0);
  if (shmCountMsgs == -1)
  {
    perror("Failed create countMsgs shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  countMsgs = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shmCountMsgs, 0);

}
void initSemMsgs()
{
  semLockMsgs = sem_open("/semLockMsgs", O_RDWR);
  if (semLockMsgs == SEM_FAILED)
  {
    perror("Failed create lock semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semLockMsg = sem_open("/semLockMsg", O_RDWR);
  if (semLockMsg == SEM_FAILED)
  {
    perror("Failed create lock semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitUpdateMsg = sem_open("/semWaitUpdateMsg", O_RDWR);
  if (semWaitUpdateMsg == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitOtherMsgs = sem_open("/semWaitOtherMsgs", O_RDWR);
  if (semWaitOtherMsgs == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitMsg = sem_open("/semWaitMsg", O_RDWR);
  if (semWaitMsg == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  semWaitBroadcastMsgs = sem_open("/semWaitBroadcastMsgs", O_RDWR);
  if (semWaitBroadcastMsgs == SEM_FAILED)
  {
    perror("Failed create wait semaphore");
    cleanAll();
    exit(EXIT_FAILURE);
  }
}
void sendMsgInChat()
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
    snprintf(msgWithNickname, MAX_LENGTH_MSG, "%s: %s",curNickname ,msg);
    sem_wait(semLockMsgs);
    strncpy(msgs+(*countMsgs * MAX_LENGTH_MSG), msgWithNickname, MAX_LENGTH_MSG);
    sem_post(semLockMsgs);

    sem_post(semWaitMsg);
  }
}
void* receiveMsgs(void* args)
{
    sem_wait(semLockMsgs);
      pthread_mutex_lock(&mutexView);
    clearMsgWin(chat);
      pthread_mutex_unlock(&mutexView);
    for(int i = 0; i < *countMsgs; i++)
    {
      pthread_mutex_lock(&mutexView);
      printMsg(chat, msgs+i*MAX_LENGTH_MSG);
      pthread_mutex_unlock(&mutexView);
    }
    sem_post(semLockMsgs);
  for(;;)
  {
    sem_wait(semWaitBroadcastMsgs);

    sem_wait(semLockMsgs);
      pthread_mutex_lock(&mutexView);
    clearMsgWin(chat);
      pthread_mutex_unlock(&mutexView);
    for(int i = 0; i < *countMsgs; i++)
    {
      pthread_mutex_lock(&mutexView);
      printMsg(chat, msgs+i*MAX_LENGTH_MSG);
      pthread_mutex_unlock(&mutexView);
    }
    sem_post(semLockMsgs);

    sem_post(semWaitUpdateMsg); //подтверждаем получение данных
    sem_wait(semWaitOtherMsgs); //ждем пока осталльные клиенты обновят данные
  }
}
int main()
{
  initShmNicknames();
  initSemNicknames();
  initShmMsgs();
  initSemMsgs();
  enterNickname();

  initChat();
  chat = createChat();
  refreshChat(chat);

  pthread_t nicknameThread;
  pthread_t msgThread;

  pthread_create(&nicknameThread, NULL, receiveNicknames, NULL);
  pthread_create(&msgThread, NULL, receiveMsgs, NULL);

  sem_wait(semLockNickname);
  strncpy(nickname, curNickname, MAX_LENGTH_NICKNAME);
  sem_post(semLockNickname);
  sem_post(semWaitNickname);

  sendMsgInChat();

  pthread_cancel(nicknameThread);
  pthread_cancel(msgThread);

  pthread_join(nicknameThread,NULL);
  pthread_join(msgThread, NULL);

  destroyChat(chat);
  cleanAll();

  return 0;
}
