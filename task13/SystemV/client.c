#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX_LENGTH_TEXT 20

int main()
{
  int res;
  int msqid;
  struct message
  {
    long type;
    char text[MAX_LENGTH_TEXT];
  } msgSnd, msgRcv;

  key_t key = ftok("server", 0);  
  if(key == -1)
  {
    perror("Failed create key");
    exit(EXIT_FAILURE);
  }

  msqid = msgget(key, 0);

  res = msgrcv(msqid, &msgRcv, sizeof(msgRcv.text), 1, 0);
  printf("%s\n", (char*) msgRcv.text);

  msgSnd.type = 2;
  strncpy(msgSnd.text, "Hello", MAX_LENGTH_TEXT);
  res = msgsnd(msqid, (void *) &msgSnd, sizeof(msgSnd.text), 0);

  return 0;
}

