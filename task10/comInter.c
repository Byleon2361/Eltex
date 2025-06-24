#include "comInter.h"
int execInput(char *input)
{
  if((input == NULL) || (input[0] == '\0')|| (input[0] == ' '))
  {
    return 1;
  }
  if (strcmp(input, "exit") == 0)
  {
    exit(EXIT_SUCCESS);
  }

  int countComms = 0;

  char ***argv = malloc(sizeof(char**)*MAX_COUNT_COMMS);
  if (argv == NULL)
  {
    perror("Failed allocate memory");
    exit(EXIT_FAILURE);
  }

  countComms = splitInputOnComms(input, argv);

  execComm(argv, countComms);
  freeArgs(argv, countComms);

  return 0;
}
int splitInputOnComms(char* input, char*** argv)
{
  int countComms = 0;

  char** tempBuf = malloc(sizeof(char*)*MAX_COUNT_ARGS);
  if (tempBuf == NULL)
  {
    perror("Failed allocate memory");
    exit(EXIT_FAILURE);
  }

  char* tokenComm = strtok(input, "|");
  while (countComms < MAX_COUNT_COMMS - 1 && tokenComm != NULL)
  {
    argv[countComms] = malloc(sizeof(char*)*MAX_COUNT_ARGS);
    if (argv[countComms] == NULL)
    {
      perror("Failed allocate memory");
      exit(EXIT_FAILURE);
    }

    tempBuf[countComms] = malloc(sizeof(char)*MAX_LENGTH_ARGS);
    if (tempBuf[countComms] == NULL)
    {
      perror("Failed allocate memory");
      exit(EXIT_FAILURE);
    }

    strncpy(tempBuf[countComms], tokenComm, MAX_LENGTH_ARGS);

    tokenComm = strtok(NULL, "|");
    countComms++;
  }
  for(int i = 0; i < countComms; i++)
  {
    splitCommsOnArgs(tempBuf[i], argv[i]);
    free(tempBuf[i]);
  }
  free(tempBuf);

  return countComms;
}
int splitCommsOnArgs(char* comm, char **argv)
{
  int argc = 0;

  char* tokenArg = strtok(comm, " ");
  while (argc < MAX_COUNT_ARGS - 1 && tokenArg != NULL)
  {
    argv[argc] = malloc(sizeof(char)*MAX_LENGTH_ARGS);
    if (argv[argc] == NULL)
    {
      perror("Failed allocate memory");
      exit(EXIT_FAILURE);
    }

    strncpy(argv[argc], tokenArg, MAX_LENGTH_ARGS);
    tokenArg = strtok(NULL, " ");
    argc++;
  }
  argv[argc] = (char*)NULL;
  return argc;
}
char *createPath(char* fullPathToApp, const char* input)
{
  if(strchr(input, '/') == NULL)
  {
    struct dirent** dir = NULL;
    int n = scandir("/usr/bin/", &dir, 0, NULL);

    if (n < 0)
    {
      perror("Can not scan dir");
      exit(EXIT_FAILURE);
    }

    for(int i = 0; i < n; i++)
    {
      if(strstr(dir[i]->d_name, input) != NULL)
      {
        strncpy(fullPathToApp, "/usr/bin/", PATH_MAX);
        strcat(fullPathToApp, input);

        for(int j =i; j < n; j++)
        {
          free(dir[j]);
        }
        free(dir);

        return fullPathToApp;
      }
      free(dir[i]);
    }
    free(dir);
  }
  else
  {
    if(input[0] == '~')
    {
      const char* homePath = getenv("HOME");
      strncpy(fullPathToApp, homePath, PATH_MAX);
      strncat(fullPathToApp, input+1, PATH_MAX);
      return fullPathToApp;
    }
  }
  strncpy(fullPathToApp, input, PATH_MAX);
  return fullPathToApp;
}
void execComm(char*** argv, int countComms)
{
  int fd[countComms - 1][2];
  pid_t pids[countComms];

  for(int i = 0; i < countComms-1; i++)
  {
    if(pipe(fd[i]) == -1)
    {
      perror("Failed pipe");
      exit(EXIT_FAILURE);
    }
  }

  for(int i = 0; i < countComms; i++)
  {
    char fullPathToApp[PATH_MAX];
    createPath(fullPathToApp, argv[i][0]);

    pids[i] = fork();
    if (pids[i] == 0)
    {
      if(i>0)
      {
        dup2(fd[i-1][0], 0);
      }
      if(i<countComms-1)
      {
        dup2(fd[i][1], 1);
      }
      for(int j = 0; j < countComms-1; j++)
      {
        close(fd[j][0]);
        close(fd[j][1]);
      }
      if (execv(fullPathToApp, argv[i]) < 0)
      {
        perror("Failed to execute program");
      }
      exit(EXIT_SUCCESS);
    }
  }
  
  for(int i = 0; i < countComms-1; i++)
  {
    close(fd[i][0]);
    close(fd[i][1]);
  }

  for(int i = 0; i < countComms; i++)
  {
    wait(NULL);
  }
}
void freeArgs(char ***argv, int countComms)
{
  for(int i = 0; i < countComms; i++)
  {
    for(int j = 0; argv[i][j] != NULL; j++)
    {
      free(argv[i][j]);
    }
    free(argv[i]);
  }
  free(argv);
}
