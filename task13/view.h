#ifndef VIEW_H
#define VIEW_H

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

extern int COLS;
extern int LINES;

typedef struct chat
{
    WINDOW* msgWin;
    WINDOW* nicknameWin;
    WINDOW* inputWin;
} Chat;

extern SCREEN* s;

void initChat();
Chat* createTable(int pos);
void refreshChat(Chat* chat);
void destroyChat(Chat* chat);
void printMsg(Chat* chat, char* msg);
void printNicknames(Chat* chat, char **nicknames, int countNicknames, char* currentNickname);
char* enterMsg(Chat* chat, char* msg, int maxLengthMsg);
#endif
