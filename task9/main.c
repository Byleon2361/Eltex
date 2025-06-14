#include "mc.h"
#include <ncurses.h>

MyWindow* tab(MyWindow* activeWin, MyWindow* left, MyWindow* right, int* y)
{
  changeStatus("Tab");
  dehighlightFile(activeWin);

  if (activeWin == left)
  {
    activeWin = right;
  }
  else
  {
    activeWin = left;
  }

  if (*y > 2 + activeWin->countFiles - 1)
  {
    *y = (activeWin->countFiles > 0) ? 2 + activeWin->countFiles - 2 : 2;
  }
  return activeWin;
}
void up(MyWindow* activeWin,int *y, int* offsetVisibleArea)
{
  changeStatus("Up");
  if (*y > 2)
    (*y)--;
  else if (*offsetVisibleArea > 0)
  {
    (*offsetVisibleArea)--;
    *y = 2;
  }
}
void down(MyWindow* activeWin,int *y, int* offsetVisibleArea)
{
  changeStatus("Down");
  int widthVisibleArea = LINES - 3;
  if (*y < 2 + widthVisibleArea - 1 && *y < activeWin->countFiles)
  {
    (*y)++;
  }
  else if (*offsetVisibleArea + widthVisibleArea < activeWin->countFiles)
  {
    (*offsetVisibleArea)++;
  }
}
int enter(MyWindow* activeWin, int* y,int* offsetVisibleArea )
{
  char fullPath[PATH_MAX];

  changeStatus("Enter");

  if (*y - 1 >= activeWin->countFiles)
  {
    changeStatus("Invalid selection");
    return 1;
  }

  snprintf(fullPath, sizeof(fullPath), "%s/%s", activeWin->path, activeWin->dir[*y - 1]->d_name);
fprintf(stderr, "%s", fullPath);//для отладки
  if (strlen(fullPath) > PATH_MAX)
  {
    changeStatus("Path too long");
    return 1;
  }

  if (activeWin->dir[*y - 1]->d_type == DT_DIR)
  {
    writePathInMyWindow(activeWin, fullPath);
    *offsetVisibleArea = 0;
    *y = 2;
  }
  else
  {
    changeStatus("It is file");
    if (execl("/usr/bin/vim", "vim", fullPath, (char*)NULL) == -1)
    {
      perror("vim");
      return 1;
    }
  }
  return 0;
}
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
  return 0;
}
