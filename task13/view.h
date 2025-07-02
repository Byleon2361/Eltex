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
int writePathInChat(Chat* chat, char *path);
void wprintDir(Chat* chat,int startVisibleAre);
int dehighlightFile(Chat* activeChat);
int highlightFile(Chat* activeWin, int y, int x);
void refreshChat(Chat* chat);
void clearMyWin(Chat* chat);
void freeNamelist(struct dirent** namelist, int count);
void destroyChat(Chat* chat);
#endif
