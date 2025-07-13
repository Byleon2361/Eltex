#include <stdio.h>
#include <unistd.h>
#include <signal.h>
int main()
{
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigprocmask(SIG_BLOCK, &set, NULL);

  int sig = 0;
  for(;;)
  {
    sigwait(&set, &sig);
    printf("Signal SIGUSR1 - %d\n", sig);
  }
  return 0;
}
