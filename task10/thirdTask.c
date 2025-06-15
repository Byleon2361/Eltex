#include "thirdTask.h"
int createArgvs(char* input, char** argv)
{
    int argc = 0;
    char* token = strtok(input, " ");
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
void execProg(char** argv)
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
void freeArgs(char** argv, int argc)
{
    for (int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
    argc = 0;
}
