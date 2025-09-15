#ifndef DRIVER_H
#define DRIVER_H
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAX_COUNT_DRIVERS 32
enum status
{
  AVAILABLE,
  BUSY,
  NONE
};
struct driver
{
  int index;
  pid_t pid;
  enum status status;
};

void init();
void cleanAll();
struct driver create_driver();
void driver_func(int index);
void send_task(pid_t, struct timeval);
enum status get_status(pid_t);
struct driver *get_driver(pid_t);
void get_drivers();
#endif
