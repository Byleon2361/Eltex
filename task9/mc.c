#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#define COUNTSUBWINS 3
extern int COLS;
extern int LINES;

WINDOW *statusWin;
typedef struct myWindow
{
  WINDOW* win;
  WINDOW* subWins[COUNTSUBWINS];
} MyWindow;
int status(WINDOW *win, int cols)
{
  statusWin=win;
  wprintw(win,"status");
  wrefresh(win);
  return OK;
}
void changeStatus(char *str)
{
  wclear(statusWin);
  wprintw(statusWin, "status: %s", str);
  wrefresh(statusWin);
}
MyWindow* createTable(int pos)
{
  MyWindow* myWin = malloc(sizeof(struct myWindow));
  int sizeCol = COLS / 6;
  WINDOW* win = newwin(LINES, COLS / 2, 0, pos);
  WINDOW* nameWin = subwin(win, LINES, sizeCol, 0, pos + sizeCol * 0);
  WINDOW* sizeWin = subwin(win, LINES, sizeCol, 0, pos + sizeCol * 1);
  WINDOW* dateWin = subwin(win, LINES, sizeCol, 0, pos + sizeCol * 2);

      wbkgd(win, COLOR_PAIR(1));
    wbkgd(nameWin, COLOR_PAIR(1));
    wbkgd(sizeWin, COLOR_PAIR(1));
    wbkgd(dateWin, COLOR_PAIR(1));

  box(win, 0, 0);
  wborder(sizeWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_TTEE, ACS_BTEE, ACS_BTEE);

      wattron(nameWin, COLOR_PAIR(3));
    mvwaddstr(nameWin, 1, 1, ".n");
    mvwaddstr(nameWin, 1, sizeCol / 2 - 1, "Name");
    wattroff(nameWin, COLOR_PAIR(3));

    wattron(sizeWin, COLOR_PAIR(3));
    mvwaddstr(sizeWin, 1, sizeCol / 2 - 1, "Size");
    wattroff(sizeWin, COLOR_PAIR(3));

    wattron(dateWin, COLOR_PAIR(3));
    mvwaddstr(dateWin, 1, sizeCol / 2 - 1, "Date");
    wattroff(dateWin, COLOR_PAIR(3));

  myWin->win = win;
  myWin->subWins[0] = nameWin;
  myWin->subWins[1] = sizeWin;
  myWin->subWins[2] = dateWin;

  refresh();
  wrefresh(win);
  return myWin;
}
void initMc()
{

  ripoffline(-1,status);
  slk_init(1);
  initscr();

  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  notimeout(stdscr, 0);

  slk_set(1, "Help", 1);
  slk_set(2, "Menu", 1);
  slk_set(3, "View", 1);
  slk_set(4, "Edit", 1);
  slk_set(5, "Copy", 1);
  slk_set(6, "Mkdir", 1);
  slk_set(7, "Delete", 1);
  slk_set(8, "Quit", 1);
  slk_refresh();

  curs_set(0);

  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE); // дефолтный цвет
  init_pair(2, COLOR_BLACK, COLOR_CYAN); //Выделенная директория или файл
  init_pair(3, COLOR_YELLOW, COLOR_BLUE); //Заголовки
}
int highlightFile(MyWindow* activeWin, int y, int x)
{
	int width = 0;
  for(int i =0;i< COUNTSUBWINS; i++ )
  {  
	  width = getmaxx(activeWin->subWins[i]) - 2;
    wchgat(activeWin->subWins[i], width,A_NORMAL,1,NULL);
    wmove(activeWin->subWins[i],y,x+1);
    wchgat(activeWin->subWins[i], width,A_NORMAL,2,NULL);
    wrefresh(activeWin->subWins[i]);
  }
  return 0;
}

int wprintDir(MyWindow* myWin, char* path)
{
  struct dirent **namelist;
  DIR* dir = opendir(path);
  if(dir == NULL)
  {
    perror("Can not read dir");
    return -1;
  }
  int n=0;
  n = scandir(path, &namelist, 0, alphasort);
  if(n < 0)
  {
    perror("Can not scan dir");
    fprintf(stderr, "errno: %d\n", errno);
    return -1;
  }  

  int offset = 2;
  int i = 0;
  for( i = 0; i < n; i++)
  {
    if(strcmp(namelist[i]->d_name, ".")==0)
    {
      continue;
    }

    mvwprintw(myWin->subWins[0],  offset++, 1, "%s", namelist[i]->d_name);
  }
  closedir(dir);
  return i;
}
void refreshMyWindow(MyWindow* win)
{
	refresh();
	wrefresh(win->win);
	wrefresh(win->subWins[0]);
	wrefresh(win->subWins[2]);
	wrefresh(win->subWins[3]);
}
int main()
{
  initMc();
  MyWindow* left = createTable(0);
  MyWindow* right = createTable(COLS/2);
  MyWindow* activeWin = left;
  int countFiles = 0;
  countFiles = wprintDir(left, ".");
  if(countFiles< 0)
  {
    changeStatus("Can not print dir on the screen");
    //return 1;
  }
  countFiles = wprintDir(left, ".");
  if(countFiles< 0)
  {
    changeStatus("Can not print dir on the screen");
    //return 1;
  }

refreshMyWindow(left);
refreshMyWindow(right);
  int x = 0;
  int y = 2;
  int ch;
  bool exit = 0;
  while (!exit)
  {
    highlightFile(activeWin, y, x);
    refresh();
    ch = getch();
    if (ch == '\t')  // tab
    {
      changeStatus("Tab");
      if(activeWin == left)
      {
        activeWin = right;
      }
      else
      {
        activeWin = left;
      }
    }
    else if (ch == 'w' || ch == KEY_UP)  // up
    {
      changeStatus("Up");
      if(y>2)
      y--;
    }
    else if (ch == 's' || ch == KEY_DOWN)  // down
    {
      changeStatus("Down");
      if(y < countFiles)
      y++;
    }
    else if (ch == '\n' || ch == '\r')  // enter
    {
      changeStatus("Enter");
      wprintDir(activeWin, ".");
    }
    else if (ch == 'q' || ch == 27)  // escape
    {
      exit = 1;
    }
    else
    {
      changeStatus("Unknow symbol");
    }
  }

  endwin();
  return 0;
}

