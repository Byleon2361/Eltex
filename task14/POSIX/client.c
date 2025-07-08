#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#define MAX_LENGTH_TEXT 20

int main()
{
  char *str = "Hello";
  int idshm = shm_open("/server", O_RDWR, 0);
  if (idshm == -1)
  {
    perror("Failed create shared memory");
    exit(EXIT_FAILURE);
  }
  ftruncate(idshm, MAX_LENGTH_TEXT);
  char *msg = mmap(NULL, MAX_LENGTH_TEXT, PROT_READ|PROT_WRITE, MAP_SHARED, idshm, 0);

  sem_t *idLockData = sem_open("/semLockData", O_RDWR);
  if (idLockData == SEM_FAILED)
  {
    perror("Failed create semaphore");
    close(idshm);
    exit(EXIT_FAILURE);
  }
  sem_t *idWait = sem_open("/semWait", O_RDWR);
  if (idWait == SEM_FAILED)
  {
    perror("Failed create semaphore");
    sem_close(idLockData);
    close(idshm);
    exit(EXIT_FAILURE);
  }

  sem_wait(idLockData);
  printf("%s\n", msg);
  strncpy(msg, str, strlen(str));
  sem_post(idLockData);

  sem_post(idWait);

  sem_close(idLockData);
  sem_close(idWait);
  close(idshm);
  return 0;
}
