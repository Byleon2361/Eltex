
#include "mcControl.h"
MyWindow* tab(MyWindow* activeWin, MyWindow* left, MyWindow* right, int* y)
{
  changeStatus("Tab\n");
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
  changeStatus("Up\n");
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
  changeStatus("Down\n");
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
void execProg(char* fullPath)
{
  pid_t pid = fork();
  if (pid == 0)
  {
    if (execl(fullPath, fullPath, (char*)NULL) < 0)
    {
      perror("Failed to execute program");
    }
    exit(0);
  }
  wait(NULL);
}
void openFileInVim(char* fullPath)
{
  pid_t pid = fork();
  if (pid == 0)
  {
    system("clear");
    if (execl("/usr/bin/vim", "vim", fullPath, (char*)NULL) < 0)
    {
      perror("Failed to open vim");
    }
    exit(0);
  }
  wait(NULL);
}
int enter(MyWindow* activeWin, int* y,int* offsetVisibleArea)
{
  char fullPath[PATH_MAX];

  changeStatus("Enter\n");

  if (*y - 1 >= activeWin->countFiles)
  {
    changeStatus("Invalid selection\n");
    return 1;
  }

  snprintf(fullPath, sizeof(fullPath), "%s/%s", activeWin->path, activeWin->dir[*y - 1]->d_name);
  if (strlen(fullPath) > PATH_MAX)
  {
    changeStatus("Path too long\n");
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
    changeStatus("It is file\n");
    if (access(fullPath, X_OK) == 0) 
    {
      execProg(fullPath);
    }
    else
    {
      openFileInVim(fullPath);
    }
  }
  return 0;
}
