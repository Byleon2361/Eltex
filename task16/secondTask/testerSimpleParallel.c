#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    double res = (double)t.tv_sec + (double)t.tv_usec * 1E-6;
    return res;
}
int main(int argc, char *argv[])
{
  char nameServerProg[NAME_MAX];
  char nameClientProg[NAME_MAX];
  int countClients = 0;

  if(argc != 4)
  {
    fprintf(stderr, "Error: count args != 4. (./tester serverName clientName 50)");
    exit(EXIT_FAILURE);
  }

  strncpy(nameServerProg, argv[1] , NAME_MAX);
  strncpy(nameClientProg, argv[2] , NAME_MAX);
  countClients = atoi(argv[3]);

  printf("%s, %s, %d\n", nameServerProg, nameClientProg, countClients);

  double t = wtime();
  pid_t pid = fork();
  if(pid == 0)
  {
    pid_t pidServer = fork();
    if(pidServer == 0)
    {
      char fullPath[PATH_MAX];
      snprintf(fullPath, PATH_MAX, "./%s", nameServerProg);
      if(execl(fullPath, nameServerProg, NULL) == -1)
      {
        perror("Failed to execute server");
        exit(EXIT_FAILURE);
      }
    }
    else if(pidServer >0)
    {
      for(int i = 0; i < countClients; i++)
      {
        pid_t pidClient = fork();
        if(pidClient == 0)
        {
          char fullPath[PATH_MAX];
          snprintf(fullPath, PATH_MAX, "./%s", nameClientProg);
          if(execl(fullPath, nameClientProg, NULL) == -1)
          {
            perror("Failed to execute client");
            exit(EXIT_FAILURE);
          }
        }
        else if(pidServer >0)
        {
          wait(NULL);
          kill(pidServer, SIGINT);
        }
      }
      wait(NULL);
    }
  }
  else if(pid >0)
  {
    wait(NULL);
    t = wtime() - t;
    printf("Time service %d clients: %0.6lf sec\n", countClients, t);
  }

  return 0;
}
