#ifndef NICKNAME_MANAGER_H
#define NICKNAME_MANAGER_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define MAX_COUNT_MESSAGES 8
#define MAX_MSG_SIZE 20

#define NICKNAME_PRIO 2
#define MAX_LENGTH_NICKNAME 20
#define MAX_COUNT_NICKNAMES 16
#define MSG_PRIO 1
#define MAX_LENGTH_MSG 256
#define MAX_COUNT_MSGS 50

typedef struct client
{
  char nickname[MAX_LENGTH_NICKNAME];
  int isActive;
} Client; 

void cleanClients();
void addClient(char *nickname);
void broadcastNicknames();
void nicknameMain();

#endif
