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

  for(int i = 0; i < countComms; i++)
  {
    printf("Аргументы команды %d\n", i);

    for(int j = 0; argv[i][j] != NULL; j++)
    {
      printf("%s\n", argv[i][j]);
    }
  }

  execComm(argv[0], argv[1]);
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
void execComm(char** argv, char** argv2)
{
  int fd[2];
  char fullPathToApp[MAX_LENGTH_FULL_PATH];
  char fullPathToApp2[MAX_LENGTH_FULL_PATH];

  strncpy(fullPathToApp, "/usr/bin/", sizeof(fullPathToApp));
  strcat(fullPathToApp, argv[0]);

  strncpy(fullPathToApp2, "/usr/bin/", sizeof(fullPathToApp2));
  strcat(fullPathToApp2, argv2[0]);

  if(pipe(fd) == -1)
  {
    perror("Failed pipe");
    exit(EXIT_FAILURE);
  }

  pid_t sender = fork();
  if (sender > 0)
  {
    pid_t reciever = fork();
    if(reciever == 0)
    {
      close(fd[1]);
      dup2(fd[0], 0);
      close(fd[0]);

      if (execv(fullPathToApp2, argv2) < 0)
      {
        perror("Failed to execute program");
      }

      exit(EXIT_SUCCESS);
    }
    wait(NULL);
  }
  else if(sender == 0)
  {
    close(fd[0]);
    dup2(fd[1], 1);
    close(fd[1]);

    if (execv(fullPathToApp, argv) < 0)
    {
      perror("Failed to execute program");
    }

    exit(EXIT_SUCCESS);
  }

  close(fd[0]);
  close(fd[1]);

  wait(NULL);
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
