#include "comInter.h"

int main()
{
  char input[MAX_LENGTH_INPUT];

  while (1)
  {
    printf("Enter the command (enter \"exit\" to exit)\n");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    execInput(input);
  }

  return 0;
}
