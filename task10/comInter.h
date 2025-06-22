#ifndef COM_INTER_H
#define COM_INTER_H

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH_INPUT 128
#define MAX_LENGTH_COMMAND 64
#define MAX_LENGTH_WORD 32
#define MAX_LENGTH_FULL_PATH 32
#define MAX_COUNT_COMMS 8
#define MAX_COUNT_ARGS 8
#define MAX_BUFFER_SIZE 1024

int execInput(char *input);
int splitInputOnComms(char* input, char** argv);
int splitCommOnArgvs(char* input, char** argv);
void execComm(char** argv, char** argv2);
void freeComms(char** comms, int countComms);
void freeArgs(char** argv, int argc);

#endif
