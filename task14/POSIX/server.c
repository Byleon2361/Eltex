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
  char *str = "Hi";
  int idshm = shm_open("/server", O_CREAT|O_RDWR, 0600);
  if (idshm == -1)
  {
    perror("Failed create shared memory");
    exit(EXIT_FAILURE);
  }
  ftruncate(idshm, MAX_LENGTH_TEXT);
  char *msg = mmap(NULL, MAX_LENGTH_TEXT, PROT_READ|PROT_WRITE, MAP_SHARED, idshm, 0);
  if(msg == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    close(idshm);
    shm_unlink("/server");
    exit(EXIT_FAILURE);
  }

  sem_t *idLockData = sem_open("/semLockData", O_CREAT|O_RDWR, 0600, 1);
  if (idLockData == SEM_FAILED)
  {
    perror("Failed create semaphore");
    munmap(msg, MAX_LENGTH_TEXT);
    close(idshm);
    shm_unlink("/server");
    exit(EXIT_FAILURE);
  }
  sem_t *idWait = sem_open("/semWait", O_CREAT|O_RDWR, 0600, 0);
  if (idWait == SEM_FAILED)
  {
    munmap(msg, MAX_LENGTH_TEXT);
    close(idshm);
    shm_unlink("/server");
    sem_close(idLockData);
    sem_unlink("/semLockData");
    perror("Failed create semaphore");
    exit(EXIT_FAILURE);
  }

  sem_wait(idLockData);
  strncpy(msg, str, strlen(str));
  sem_post(idLockData);

  sem_wait(idWait);
  printf("%s\n", msg);

  munmap(msg, MAX_LENGTH_TEXT);
  close(idshm);
  sem_close(idLockData);
  sem_close(idWait);
  shm_unlink("/server");
  sem_unlink("/semLockData");
  sem_unlink("/semWait");
  return 0;
}
