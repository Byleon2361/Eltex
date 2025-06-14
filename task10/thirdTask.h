#ifndef THIRD_TASK_H
#define THIRD_TASK_H

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH_INPUT 32
#define MAX_LENGTH_WORD 16
#define MAX_LENGTH_FULL_PATH 32
#define MAX_COUNT_ARGS 16
int createArgvs(char* input, char** argv);
void execProg(char** argv);
void freeArgs(char** argv, int argc);

#endif
