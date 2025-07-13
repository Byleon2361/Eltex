#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#define SIGINT 2
int main()
{
  sigset_t sig;
  sigemptyset(&sig);
  sigaddset(&sig, SIGINT);
  sigprocmask(SIG_BLOCK, &sig, NULL);

  for(;;)
  {
    printf("loop\n");
    sleep(1);
  }
  return 0;
}
