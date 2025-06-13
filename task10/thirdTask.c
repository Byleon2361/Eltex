#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH_INPUT 32
#define MAX_LENGTH_WORD 16
#define MAX_LENGTH_FULL_PATH 32
#define MAX_COUNT_ARGS 16

int main()
{
    char* argv[MAX_COUNT_ARGS];
    char input[MAX_LENGTH_INPUT];
    char* token;
    int argc = 0;

    char fullPathToApp[MAX_LENGTH_FULL_PATH];
    pid_t pid;

    while (1)
    {
        printf("Enter the command (enter \"exit\" to exit)\n");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; 

        token = strtok(input, " ");
        while (argc < MAX_COUNT_ARGS - 1 && token != NULL)
        {
            argv[argc] = malloc(MAX_LENGTH_WORD);
            if (argv[argc] == NULL)
            {
                perror("Failed allocate memory");
                return 1;
            }
            strncpy(argv[argc], token, MAX_LENGTH_WORD);
            token = strtok(NULL, " ");
            argc++;
        }
        argv[argc] = (char*)NULL;

        if (strcmp(argv[0], "exit") == 0)
        {
          for (int i=0; i < argc; i++)
          {
            free(argv[i]);
          }
          break;
        }

        strncpy(fullPathToApp, "/usr/bin/", sizeof(fullPathToApp));
        strcat(fullPathToApp, argv[0]);

        pid = fork();
        if (pid == 0)
        {
            if (execv(fullPathToApp, argv) < 0)
            {
                perror("Failed to execute program");
            }
            exit(0);
        }
        wait(NULL);
    for (int i=0; i < argc; i++)
    {
        free(argv[i]);
    }
    argc = 0;
    }

    return 0;
}
