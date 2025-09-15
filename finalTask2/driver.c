#include "driver.h"

struct driver drivers[MAX_COUNT_DRIVERS];
int driverPipes[MAX_COUNT_DRIVERS][2];
int statusPipes[MAX_COUNT_DRIVERS][2];

int countDrivers = 0;

void signal_handler(int sig)
{
  exit(EXIT_SUCCESS);
}
void init()
{
  for(int i = 0; i < MAX_COUNT_DRIVERS; i++)
  {
    if(pipe(driverPipes[i]) == -1)
    {
      perror("Error create pipe");
      exit(EXIT_FAILURE);
    }
    if(pipe(statusPipes[i]) == -1)
    {
      perror("Error create pipe");
      exit(EXIT_FAILURE);
    }
  }
}
void cleanAll()
{
  for(int i = 0; i < countDrivers; i++)
  {
    close(driverPipes[i][0]);
    close(driverPipes[i][1]);
    close(statusPipes[i][0]);
    close(statusPipes[i][1]);
    kill(drivers[i].pid, SIGTERM);
    waitpid(drivers[i].pid, NULL, 0);
  }
}
struct driver create_driver()
{
  struct driver driver;
  pid_t pid = fork();

  if(pid == 0)
  {
    close(driverPipes[countDrivers][1]);
    close(statusPipes[countDrivers][0]);
    driver_func(countDrivers);
    exit(EXIT_SUCCESS);
  }
  else if (pid > 0)
  {
    if(countDrivers < MAX_COUNT_DRIVERS)
    {
      driver.pid = pid;
      driver.status = AVAILABLE;
      driver.index = countDrivers;

      close(driverPipes[countDrivers][0]);
      close(statusPipes[countDrivers][1]);

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
  signal(SIGTERM, signal_handler);

  struct timeval tv;
  fd_set readfds;

  for(;;)
  {
    FD_ZERO(&readfds);
    FD_SET(driverPipes[index][0], &readfds);

    int active = select(driverPipes[index][0]+1, &readfds, NULL, NULL, NULL);
    if(active == -1)
    {
      perror("Error in select");
      exit(EXIT_FAILURE);
    }
    if(active > 0 && FD_ISSET(driverPipes[index][0], &readfds))
    {
      int readBytes = read(driverPipes[index][0], &tv, sizeof(tv));

      enum status busyStatus = BUSY;
      write(statusPipes[index][1], &busyStatus, sizeof(busyStatus));

      sleep(tv.tv_sec);

      enum status availableStatus = AVAILABLE;
      write(statusPipes[index][1], &availableStatus, sizeof(availableStatus));

    }
  }
}
void send_task(pid_t pid, struct timeval tv)
{
  struct driver *driver = get_driver(pid);
  if(driver)
  {
    write(driverPipes[driver->index][1], &tv, sizeof(tv));
  }
}
enum status get_status(pid_t pid)
{
  struct driver *driver = get_driver(pid);
  fd_set readfds;
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  enum status curStatus;
  if(driver)
  {
    FD_ZERO(&readfds);
    FD_SET(statusPipes[driver->index][0], &readfds);

    int active = select(statusPipes[driver->index][0]+1, &readfds, NULL, NULL, &timeout);

    if(active > 0 && FD_ISSET(statusPipes[driver->index][0], &readfds))
    {
      read(statusPipes[driver->index][0], &curStatus, sizeof(curStatus));
      driver->status = curStatus;


    }
    return driver->status;
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
  return NULL;
}
void get_drivers()
{

  printf("-----------------------\n");
  printf("Количество процессов: %d\n", countDrivers);
  for(int i = 0; i < countDrivers; i++)
  {
    get_status(drivers[i].pid);
    printf("%d) Номер процесса: %d; Статус процесса: %d; Индекс процесса: %d\n", i+1, drivers[i].pid, drivers[i].status, drivers[i].index);
  }
  printf("-----------------------\n");
}
