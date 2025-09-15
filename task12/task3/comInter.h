#ifndef COM_INTER_H
#define COM_INTER_H

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_LENGTH_INPUT 2048
#define MAX_LENGTH_ARGS PATH_MAX
#define MAX_COUNT_COMMS 8
#define MAX_COUNT_ARGS 8

int execInput(char *input);
int splitInputOnComms(char* input, char*** argv);
int splitCommsOnArgs(char* comm, char **argv);
void execComm(char*** argv, int countComms);
void freeArgs(char ***argv, int countComms);
#endif
