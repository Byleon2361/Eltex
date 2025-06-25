#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <unistd.h>
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
  } msgSnd , msgRcv;

  key_t key = ftok("server", 0);  

  msqid = msgget(key, IPC_CREAT|0600);

  msgSnd.type = 1;
  strncpy(msgSnd.text, "Hi", MAX_LENGTH_TEXT);
  res = msgsnd(msqid, (void *) &msgSnd, sizeof(msgSnd.text), 0);

  res = msgrcv(msqid, &msgRcv, sizeof(msgRcv.text), 2, 0);
  printf("%s\n", (char*) msgRcv.text);

  return 0;
}
