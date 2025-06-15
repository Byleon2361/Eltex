#ifndef MC_VISUAL_H
#define MC_VISUAL_H

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
    struct dirent **dir;
    char path[PATH_MAX];
    int countFiles;
} MyWindow;

extern WINDOW* statusWin;
extern SCREEN* s;

int status(WINDOW* win, int cols);
void changeStatus(char* str);
void initMc();
MyWindow* createTable(int pos);
int writePathInMyWindow(MyWindow* myWin, char *path);
void wprintDir(MyWindow* myWin,int startVisibleAre);
int dehighlightFile(MyWindow* activeWin);
int highlightFile(MyWindow* activeWin, int y, int x);
void refreshMyWindow(MyWindow* win);
void clearMyWin(MyWindow* myWin);
void freeNamelist(struct dirent** namelist, int count);
void destroyMyWindow(MyWindow* win);
#endif
