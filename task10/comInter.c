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
    /* printf("%s\n\n", comms[i]); */

    argc = splitCommOnArgvs(comms[i], argv);
    /* for(int j = 0; j < argc; j++) */
    /* { */
    /*   printf("%s\n", argv[j]); */
    /* } */
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
void execComm(char** argv)
{
    char fullPathToApp[MAX_LENGTH_FULL_PATH];

    strncpy(fullPathToApp, "/usr/bin/", sizeof(fullPathToApp));
    strcat(fullPathToApp, argv[0]);

    pid_t pid = fork();
    if (pid == 0)
    {
        if (execv(fullPathToApp, argv) < 0)
        {
            perror("Failed to execute program");
        }
        exit(0);
    }
    wait(NULL);
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
