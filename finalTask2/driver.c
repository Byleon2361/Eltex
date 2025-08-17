#include "driver.h"
/* SHARED MEMORY ID */
int shmTimevals;
int shmDrivers;
/* SHARED MEMORY*/
struct timeval *timevals; 
struct driver *drivers;

int countDrivers = 0;
sigset_t set;
int sig = 0;
void init()
{
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigprocmask(SIG_BLOCK, &set, NULL);

  shmTimevals = shm_open("/shmTimevals", O_CREAT|O_RDWR, 0600);
  if (shmTimevals == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  ftruncate(shmTimevals, MAX_COUNT_DRIVERS*sizeof(timevals[0]));
  timevals = mmap(NULL, MAX_COUNT_DRIVERS*sizeof(timevals[0]), PROT_READ|PROT_WRITE, MAP_SHARED, shmTimevals, 0);
  if (timevals == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  memset(timevals, 0, MAX_COUNT_DRIVERS*sizeof(timevals[0]));

  shmDrivers = shm_open("/shmDrivers", O_CREAT|O_RDWR, 0600);
  if (shmDrivers == -1)
  {
    perror("Failed create shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  ftruncate(shmDrivers, MAX_COUNT_DRIVERS*sizeof(drivers[0]));
  drivers = mmap(NULL, MAX_COUNT_DRIVERS*sizeof(drivers[0]), PROT_READ|PROT_WRITE, MAP_SHARED, shmDrivers, 0);
  if (drivers == MAP_FAILED)
  {
    perror("Failed allocate shared memory");
    cleanAll();
    exit(EXIT_FAILURE);
  }
  memset(timevals, 0, MAX_COUNT_DRIVERS*sizeof(timevals[0]));
}
void cleanAll()
{
  munmap(timevals, MAX_COUNT_DRIVERS*sizeof(timevals[0]));
  munmap(drivers, MAX_COUNT_DRIVERS*sizeof(drivers[0]));

  close(shmTimevals);
  shm_unlink("/shmTimevals");
  close(shmDrivers);
  shm_unlink("/shmDrivers");

  for(int i = 0; i < countDrivers; i++)
  {
    kill(drivers[i].pid, SIGINT);
  }
}
struct driver create_driver()
{
  struct driver driver;
  pid_t pid = fork();
  if(pid == 0)
  {
    driver_func(countDrivers);
  }
  else if (pid > 0)
  {
    if(countDrivers < MAX_COUNT_DRIVERS)
    {
      driver.pid = pid;
      driver.status = AVAILABLE;
      driver.index = countDrivers;

      drivers[countDrivers] = driver;
      countDrivers++;
      printf("Процесс создан pid=%d\n", pid);
    }
    else
      printf("Достигнуто макисмальное количество водителей\n");
  }
  return driver;
}
void driver_func(int index)
{
  struct timeval tv;
  for(;;)
  {
    sigwait(&set, &sig);
    drivers[index].status = BUSY;
    tv = timevals[index];
    sleep(tv.tv_sec);
    drivers[index].status = AVAILABLE;
  }
}
void send_task(pid_t pid, struct timeval tv)
{
  struct driver *driver = get_driver(pid);
  timevals[driver->index] = tv;
  kill(pid, SIGUSR1);
}
enum status get_status(pid_t pid)
{
  for(int i = 0; i < countDrivers; i++)
  {
    if(drivers[i].pid == pid)
    {
      return drivers[i].status;
    }
  }
  return NONE;
}
struct driver *get_driver(pid_t pid)
{
  for(int i = 0; i < countDrivers; i++)
  {
    if(drivers[i].pid == pid)
    {
      return &drivers[i];
    }
  }
}
void get_drivers()
{
  printf("-----------------------\n");
  printf("Количество процессов: %d\n", countDrivers);
  for(int i = 0; i < countDrivers; i++)
  {
    printf("%d) Номер процесса: %d; Статус процесса: %d; Индекс процесса: %d\n", i+1, drivers[i].pid, drivers[i].status, drivers[i].index);
  }
  printf("-----------------------\n");
}
