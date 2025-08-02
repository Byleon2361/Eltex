#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#define FILE_NAME "test.txt"
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
  int countSuccess = 0;
  FILE *file = fopen(FILE_NAME, "a");
  if(file == NULL)
  {
    perror("Error create file");
    exit(EXIT_FAILURE);
  }
  fprintf(file, "-----------------------------------\n");
  if(argc != 4)
  {
    fprintf(stderr, "Error: count args != 4. (./tester serverName clientName 50)");
    exit(EXIT_FAILURE);
  }

  strncpy(nameServerProg, argv[1] , NAME_MAX);
  strncpy(nameClientProg, argv[2] , NAME_MAX);
  countClients = atoi(argv[3]);

  printf("%s, %s, %d\n", nameServerProg, nameClientProg, countClients);

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
    usleep(100000);
  double t = wtime();
    pid_t *allClientPids = malloc(countClients * sizeof(pid_t)); 
    int i = 0;
    for(i = 0; i < countClients; i++)
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
      else if(pidClient > 0)
      {
        allClientPids[i] = pidClient;
      }
      else
      {
        int status = 0;
        for(int j = 0; j < i; j++)
        {
          waitpid(allClientPids[j], &status, 0);
          if(WEXITSTATUS(status) == EXIT_SUCCESS)
            countSuccess++;
        }

        perror("Error fork client");
        free(allClientPids);
        t = wtime() - t;
        fprintf(file,"Server crushed. Server: %s; Count try clients: %d; Count finished clients: %d; Time: %0.6lf sec; Count success: %d; Count failed: %d.\n", argv[1], countClients,  i, t, countSuccess, countClients-countSuccess);
        exit(EXIT_FAILURE);
      }
    }
    int status = 0;
    for(int i = 0; i < countClients; i++)
    {
      waitpid(allClientPids[i], &status, 0);
      if(WEXITSTATUS(status) == EXIT_SUCCESS)
        countSuccess++;
    }
    free(allClientPids);
    kill(pidServer, SIGTERM);
    waitpid(pidServer, NULL, 0);
  t = wtime() - t;
        fprintf(file,"Server: %s; Count finished clients: %d; Time: %0.6lf sec; Count success: %d; Count failed: %d.\n", argv[1], i, t, countSuccess, countClients-countSuccess);
    exit(EXIT_SUCCESS);
  }
  else
  {
    perror("Error fork server");
    exit(EXIT_FAILURE);
  }

  return 0;
}
