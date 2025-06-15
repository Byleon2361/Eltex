#include "thirdTask.h"

int main()
{
    char* argv[MAX_COUNT_ARGS];
    char input[MAX_LENGTH_INPUT];
    int argc = 0;

    while (1)
    {
        printf("Enter the command (enter \"exit\" to exit)\n");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        argc = createArgvs(input, argv);
        if (strcmp(argv[0], "exit") == 0)
        {
            freeArgs(argv, argc);
            break;
        }

        execProg(argv);
        freeArgs(argv, argc);
        argc = 0;
    }

    return 0;
}
