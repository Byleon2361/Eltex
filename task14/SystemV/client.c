#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#define MAX_LENGTH_TEXT 20

int main()
{
  char *str = "Hello";

  key_t shmKey = ftok("server", 0);  
  if(shmKey == -1)
  {
    perror("Failed create key for shared memory");
    exit(EXIT_FAILURE);
  }
  int shmid = shmget(shmKey,MAX_LENGTH_TEXT,0);
  if(shmid == -1)
  {
    perror("Failed open shared memory id");
    exit(EXIT_FAILURE);
  }

  key_t semKey = ftok("server", 1);  
  if(semKey == -1)
  {
    perror("Failed create key for semaphore");
    exit(EXIT_FAILURE);
  }
  int semid = semget(semKey,2,0);
  if(shmid == -1)
  {
    perror("Failed open semaphore id");
    exit(EXIT_FAILURE);
  }
  struct sembuf lock[2] = {{0,0,0},{0,1,0}};
  struct sembuf unlock = {0,-1,0};

  struct sembuf wait = {1,-1,0};
  struct sembuf nowait = {1,1,0};

  char *msgShm1 = shmat(shmid, NULL, 0);

  semop(semid,&lock[0], 2);
  printf("%s\n", msgShm1);
  strncpy(msgShm1, str, strlen(str));
  semop(semid,&unlock, 1);

  semop(semid,&nowait, 1);
  
  shmdt(msgShm1);

  return 0;
}
