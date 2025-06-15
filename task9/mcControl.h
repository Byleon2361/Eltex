
#ifndef MC_CONTROL_H
#define MC_CONTROL_H
#include "mcVisual.h"
#include  <sys/wait.h>
MyWindow* tab(MyWindow* activeWin, MyWindow* left, MyWindow* right, int* y);
void up(MyWindow* activeWin,int *y, int* offsetVisibleArea);
void down(MyWindow* activeWin,int *y, int* offsetVisibleArea);
void execProg(char* fullPath);
void openFileInVim(char* fullPath);
int enter(MyWindow* activeWin, int* y,int* offsetVisibleArea );
#endif
