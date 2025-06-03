#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
extern int COLS;
extern int LINES;
int main()
{
    SCREEN* s;
    WINDOW* left;
    WINDOW* right;

    slk_init(1);
    s = newterm(NULL, stdout, stdin);
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

    int sizeCol = COLS / 6;

    left = newwin(LINES, COLS / 2, 0, 0);
    WINDOW* nameLeft = subwin(left, LINES, sizeCol, 0, sizeCol * 0);
    WINDOW* sizeLeft = subwin(left, LINES, sizeCol, 0, sizeCol * 1);
    WINDOW* dateLeft = subwin(left, LINES, sizeCol, 0, sizeCol * 2);

    box(left, 0, 0);
    wborder(sizeLeft, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_TTEE, ACS_BTEE, ACS_BTEE);
    // wbkgd(nameLeft, '1');
    // wbkgd(sizeLeft, '2');
    // wbkgd(dateLeft, '3');

    right = newwin(LINES, COLS / 2, 0, COLS / 2);
    WINDOW* nameRight = subwin(right, LINES, sizeCol, 0, COLS / 2 + sizeCol * 0);
    WINDOW* sizeRight = subwin(right, LINES, sizeCol, 0, COLS / 2 + sizeCol * 1);
    WINDOW* dateRight = subwin(right, LINES, sizeCol, 0, COLS / 2 + sizeCol * 2);

    box(right, 0, 0);
    wborder(sizeRight, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_TTEE, ACS_BTEE, ACS_BTEE);
    // wbkgd(nameRight, '4');
    // wbkgd(sizeRight, '5');
    // wbkgd(dateRight, '6');

    mvwaddstr(nameLeft, 1, 1, ".n");
    mvwaddstr(nameLeft, 1, sizeCol / 2 - 1, "Name");
    mvwaddstr(sizeLeft, 1, sizeCol / 2 - 1, "Size");
    mvwaddstr(dateLeft, 1, sizeCol / 2 - 1, "Date");
    mvwaddstr(nameRight, 1, 1, ".n");
    mvwaddstr(nameRight, 1, sizeCol / 2 - 1, "Name");
    mvwaddstr(sizeRight, 1, sizeCol / 2 - 1, "Size");
    mvwaddstr(dateRight, 1, sizeCol / 2 - 1, "Date");

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    wattr_on(stdscr, COLOR_PAIR(1), NULL);

    refresh();
    wrefresh(left);
    wrefresh(right);

    int ch;
    bool exit = 0;
    while (!exit)
    {

        ch = getch();
        if (ch == '\t')  // tab
        {
            printf("tab|");
        }
        else if (ch == 'w' || ch == KEY_UP)  // up
        {
            printf("up|");
        }
        else if (ch == 's' || ch == KEY_DOWN)  // down
        {
            printf("down|");
        }
        else if (ch == '\n' || ch == '\r')  // enter
        {
            printf("enter|");
        }
        else if (ch == 'q' || ch == 27)  // escape
        {
            exit = 1;
        }
        else
        {
            printf("Непонятный символ|");
        }
        refresh();
    }

    endwin();
    delscreen(s);
    return 0;
}