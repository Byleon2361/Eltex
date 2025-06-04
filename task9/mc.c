#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <dirent.h>
extern int COLS;
extern int LINES;

WINDOW *statusWin;
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
WINDOW* createTable(int pos)
{

    int sizeCol = COLS / 6;
    WINDOW* win = newwin(LINES, COLS / 2, 0, pos);
    WINDOW* nameWin = subwin(win, LINES, sizeCol, 0, pos + sizeCol * 0);
    WINDOW* sizeWin = subwin(win, LINES, sizeCol, 0, pos + sizeCol * 1);
    WINDOW* dateWin = subwin(win, LINES, sizeCol, 0, pos + sizeCol * 2);

    box(win, 0, 0);
    wborder(sizeWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_TTEE, ACS_BTEE, ACS_BTEE);
    // wbkgd(nameRight, '4');
    // wbkgd(sizeRight, '5');
    // wbkgd(dateRight, '6');

    mvwaddstr(nameWin, 1, 1, ".n");
    mvwaddstr(nameWin, 1, sizeCol / 2 - 1, "Name");
    mvwaddstr(sizeWin, 1, sizeCol / 2 - 1, "Size");
    mvwaddstr(dateWin, 1, sizeCol / 2 - 1, "Date");
    return win;
}
void initMc()
{
    WINDOW* left;
    WINDOW* right;

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
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    wattr_on(stdscr, COLOR_PAIR(1), NULL);

}
int wprintDir()
{
  DIR* dir = opendir(".");
  if(dir == NULL)
  {
    return 1;
  }
  
  closedir(dir);
  return 0;
}

int main()
{
    initMc();
    WINDOW* right = createTable(0);
    WINDOW* left = createTable(COLS/2);
    refresh();
    wrefresh(left);
    wrefresh(right);
//    wprintDir();
    int ch;
    bool exit = 0;
    while (!exit)
    {

        ch = getch();
        if (ch == '\t')  // tab
        {
            changeStatus("Tab");
        }
        else if (ch == 'w' || ch == KEY_UP)  // up
        {
            changeStatus("Up");
        }
        else if (ch == 's' || ch == KEY_DOWN)  // down
        {
            changeStatus("Down");
        }
        else if (ch == '\n' || ch == '\r')  // enter
        {
            changeStatus("Enter");
        }
        else if (ch == 'q' || ch == 27)  // escape
        {
            exit = 1;
        }
        else
        {
            changeStatus("Unknow symbol");
        }
        refresh();
    }

    endwin();
    return 0;
}

