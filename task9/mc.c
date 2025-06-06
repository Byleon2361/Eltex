#include "mc.h"
WINDOW* statusWin = NULL;
int status(WINDOW* win, int cols)
{
    statusWin = win;
    wprintw(win, "status");
    wrefresh(win);
    return OK;
}
void changeStatus(char* str)
{
    wclear(statusWin);
    wprintw(statusWin, "status: %s", str);
    wrefresh(statusWin);
}
void initMc()
{
    ripoffline(-1, status);
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
    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // дефолтный цвет
    init_pair(2, COLOR_BLACK, COLOR_CYAN);   // Выделенная директория или файл
    init_pair(3, COLOR_YELLOW, COLOR_BLUE);  // Заголовки
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
    refreshMyWindow(myWin);
    return myWin;
}
int wprintDir(MyWindow* myWin, struct dirent*** namelist, char* path, int startVisibleArea)
{
    int n = scandir(path, namelist, 0, alphasort);
    if (n < 0)
    {
        perror("Can not scan dir");
        fprintf(stderr, "errno: %d\n", errno);
        return -1;
    }

    clearMyWin(myWin);

    struct stat fileInfo;
    char pathToFile[PATH_MAX];
    char time_str[20];
    int offset = 2;
    int widthVisibleArea = LINES -3;
    int endVisivleArea = startVisibleArea + widthVisibleArea;
    if(endVisivleArea > n)
      endVisivleArea = n;
    
    for (int i = startVisibleArea; i < endVisivleArea; i++)
    {
        if (strcmp((*namelist)[i]->d_name, ".") == 0)
        {
            continue;
        }

        snprintf(pathToFile, sizeof(pathToFile), "%s/%s", path, (*namelist)[i]->d_name);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", localtime(&fileInfo.st_ctime));

        mvwprintw(myWin->subWins[0], offset, 1, "%s", (*namelist)[i]->d_name);
        if (stat(pathToFile, &fileInfo) == 0)
        {
            mvwprintw(myWin->subWins[1], offset, 1, "%ld", (long)fileInfo.st_size);
            mvwprintw(myWin->subWins[2], offset, 1, "%s", time_str);
        }
        else
        {
            perror("stat");
        }
        offset++;
    }
    return n;
}
int dehighlightFile(MyWindow* activeWin)
{
    int width = 0;
    for (int i = 0; i < COUNT_SUBWINS; i++)
    {
        width = getmaxx(activeWin->subWins[i]) - 2;
        wchgat(activeWin->subWins[i], width, A_NORMAL, 1, NULL);  // затираем предыдущую
        wrefresh(activeWin->subWins[i]);
    }
    box(activeWin->win, 0, 0);
    wborder(activeWin->subWins[1], ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_TTEE, ACS_BTEE, ACS_BTEE);
    refreshMyWindow(activeWin);
    return 0;
}
int highlightFile(MyWindow* activeWin, int y, int x)
{

    int width = 0;
    dehighlightFile(activeWin);
    for (int i = 0; i < COUNT_SUBWINS; i++)
    {
        int width = getmaxx(activeWin->subWins[i]) - 2;
        mvwchgat(activeWin->subWins[i], y, 1, width, A_REVERSE, 1, NULL);
        wrefresh(activeWin->subWins[i]);
    }
    return 0;
}
void refreshMyWindow(MyWindow* win)
{
    refresh();
    wrefresh(win->win);
    wrefresh(win->subWins[0]);
    wrefresh(win->subWins[1]);
    wrefresh(win->subWins[2]);
}
void clearMyWin(MyWindow* myWin)
{
    int start_line = 2;
    for (int i = start_line; i < LINES; i++)
    {
        wmove(myWin->subWins[0], i, 1);
        wclrtoeol(myWin->subWins[0]);

        wmove(myWin->subWins[1], i, 1);
        whline(myWin->subWins[1], ' ', getmaxx(myWin->subWins[1]));

        wmove(myWin->subWins[2], i, 0);
        wclrtoeol(myWin->subWins[2]);
    }
    box(myWin->win, 0, 0);
    wborder(myWin->subWins[1], ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_TTEE, ACS_BTEE, ACS_BTEE);
    refreshMyWindow(myWin);
}
void freeNamelist(struct dirent** namelist, int count)
{
    if (!namelist) return;
    for (int i = 0; i < count; i++)
    {
        free(namelist[i]);
    }
    free(namelist);
}
