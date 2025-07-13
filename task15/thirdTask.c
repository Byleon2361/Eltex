#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#define SIGUSR1 10
void handler(int sig)
{
  printf("SIGUSR1\n");
}
int main()
{
  struct sigaction sigact;
  sigact.sa_handler = handler;

  sigaction(SIGUSR1,&sigact, NULL);
  for(;;)
  {
    printf("loop\n");
    sleep(1);
  }
  return 0;
}
