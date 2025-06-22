#include "comInter.h"
int execInput(char *input)
{
  if((input == NULL) || (input[0] == '\0')|| (input[0] == ' '))
  {
    return 1;
  }
  char* comms[MAX_COUNT_COMMS];
  int countComms = 0;
  char* argv[MAX_COUNT_ARGS];
  int argc = 0;

  countComms = splitInputOnComms(input, comms);
  for(int i = 0; i < countComms; i++)
  {
    printf("%s\n\n", comms[i]);

    argc = splitCommOnArgvs(comms[i], argv);
    for(int j = 0; j < argc; j++)
    {
      printf("%s\n", argv[j]);
    }
    argc = 0;
  }
  if (strcmp(argv[0], "exit") == 0)
  {
    freeComms(comms, countComms);
    freeArgs(argv, argc);
    exit(EXIT_SUCCESS);
  }

  /* execComm(argv); */
  freeComms(comms, countComms);
  freeArgs(argv, argc);
  argc = 0;
  
  return 0;
}
int splitInputOnComms(char* input, char** comms)
{
    int countComms = 0;
    char* token = strtok(input, "|");
    while (countComms < MAX_COUNT_COMMS - 1 && token != NULL)
    {
        comms[countComms] = malloc(MAX_LENGTH_COMMAND);
        if (comms[countComms] == NULL)
        {
            perror("Failed allocate memory");
            exit(1);
        }
        strncpy(comms[countComms], token, MAX_LENGTH_COMMAND);
        token = strtok(NULL, "|");
        countComms++;
    }
    comms[countComms] = (char*)NULL;
    return countComms;
}
int splitCommOnArgvs(char* comm, char** argv)
{
    int argc = 0;
    char* token = strtok(comm, " ");
    while (argc < MAX_COUNT_ARGS - 1 && token != NULL)
    {
        argv[argc] = malloc(MAX_LENGTH_WORD);
        if (argv[argc] == NULL)
        {
            perror("Failed allocate memory");
            exit(1);
        }
        strncpy(argv[argc], token, MAX_LENGTH_WORD);
        token = strtok(NULL, " ");
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
    char buf[MAX_BUFFER_SIZE];

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
    pid_t reciever = fork();
    if (sender == 0)
    {
        if (execv(fullPathToApp, argv) < 0)
        {
          perror("Failed to execute program");
        }
        
    dup2(stdout, fd[1]);
        close(fd[0]);
        write(fd[1], buf, MAX_BUFFER_SIZE);
        close(fd[1]);
        exit(EXIT_SUCCESS);
    }
    else if(reciever == 0)
    {
      if (execv(fullPathToApp2, argv2) < 0)
      {
        perror("Failed to execute program");
      }

    dup2(stdin, fd[0]);
      close(fd[1]);
      read(fd[0], buf, MAX_BUFFER_SIZE);
      close(fd[0]);
      exit(EXIT_SUCCESS);
    }
    wait(NULL);
    wait(NULL);

    dup2(fd[1], stdout);
    dup2(fd[0], stdin);
}
void freeComms(char** comms, int countComms)
{
    for (int i = 0; i < countComms; i++)
    {
      free(comms[countComms]);
    }
}
void freeArgs(char** argv, int argc)
{
    for (int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
}
