#include "mcVisual.h"
#include "mcControl.h"
#include <ncurses.h>
#include <sys/wait.h>
int main()
{
  initMc();

  MyWindow* left = createTable(0);
  MyWindow* right = createTable(COLS / 2);
  if(writePathInMyWindow(left,".")<0)
  {
    fprintf(stderr, "Failed write path in the struct\n");
    return 1;
  }
  if(writePathInMyWindow(right, ".")<0)
  {
    fprintf(stderr, "Failed write path in the struct\n");
    return 1;
  }

  int x = 0;
  int y = 2;
  int offsetVisibleArea = 0;
  int ch;
  bool exit = 0;

  wprintDir(left, offsetVisibleArea);
  wprintDir(right, offsetVisibleArea);
  refreshMyWindow(left);
  refreshMyWindow(right);
  refresh();

  MyWindow* activeWin = left;
  while (!exit)
  {
    wprintDir(activeWin, offsetVisibleArea);
    highlightFile(activeWin, y, x);
    refreshMyWindow(activeWin);

    ch = getch();

    if (ch == '\t')  // tab
    {
      activeWin = tab(activeWin, left, right, &y);
    }
    else if (ch == 'w' || ch == 'k' || ch == KEY_UP)  // up
    {
      up(activeWin, &y, &offsetVisibleArea);
    }
    else if (ch == 's' || ch == 'j' || ch == KEY_DOWN)  // down
    {
      down(activeWin, &y, &offsetVisibleArea);
    }
    else if (ch == '\n' || ch == '\r')  // enter
    {
      if(enter(activeWin, &y, &offsetVisibleArea) > 0)
      {
        fprintf(stderr, "Error enter\n");
        break;
      }
    }
    else if (ch == 'q' || ch == 27)  // escape
    {
      exit = 1;
    }
    else
    {
      changeStatus("Unknown symbol");
    }
  }

  destroyMyWindow(left);
  destroyMyWindow(right);

  delwin(statusWin);
  slk_restore();
  resetty();
  clear();
  refresh();
  endwin();
  delscreen(s);
  return 0;
}
