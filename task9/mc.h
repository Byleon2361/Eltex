#ifndef MC_H 
#define MC_H

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define COUNT_SUBWINS 3

extern int COLS;
extern int LINES;

typedef struct myWindow
{
    WINDOW* win;
    WINDOW* subWins[COUNT_SUBWINS];
} MyWindow;

extern WINDOW* statusWin;

int status(WINDOW* win, int cols);
void changeStatus(char* str);
void initMc();
MyWindow* createTable(int pos);
int wprintDir(MyWindow* myWin, struct dirent*** namelist, char* path, int startVisibleAre);
int dehighlightFile(MyWindow* activeWin);
int highlightFile(MyWindow* activeWin, int y, int x);
void refreshMyWindow(MyWindow* win);
void clearMyWin(MyWindow* myWin);
void freeNamelist(struct dirent** namelist, int count);

#endif
