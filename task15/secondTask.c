#include <stdio.h>
#include <unistd.h>
#include <signal.h>
int main()
{
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  sigprocmask(SIG_BLOCK, &set, NULL);

  for(;;)
  {
    printf("loop\n");
    sleep(1);
  }
  return 0;
}
