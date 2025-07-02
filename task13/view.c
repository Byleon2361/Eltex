#include "view.h"
SCREEN* s = NULL;
void initChat()
{
    s = newterm(NULL,stdout, stdin);

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    notimeout(stdscr, 0);

    curs_set(0);

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // Дефолтный цвет
    init_pair(2, COLOR_BLACK, COLOR_CYAN);   // Выделенная директория или файл
    init_pair(3, COLOR_YELLOW, COLOR_BLUE);  // Заголовки
}
Chat* createChat()
{
    Chat* chat = malloc(sizeof(Chat));
    int sizeCol = COLS / 6;

    WINDOW* msgWin = newwin(LINES-3, COLS*4 / 6, 0, 0);
    WINDOW* nicknameWin = newwin(LINES-3, COLS*2 / 6, 0, COLS*4/6);
    WINDOW* inputWin = newwin(3, COLS, LINES-3, 0);

    wbkgd(msgWin, COLOR_PAIR(1));
    wbkgd(nicknameWin, COLOR_PAIR(1));
    wbkgd(inputWin, COLOR_PAIR(1));

    wborder(msgWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wborder(nicknameWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wborder(inputWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

    chat->msgWin = msgWin;
    chat->nicknameWin = nicknameWin;
    chat->inputWin = inputWin;

    refresh();
    refreshChat(chat);

    return chat;
}
/* int highlightFile(Chat* activeWin, int y, int x) */
/* { */

/*     int width = 0; */
/*     dehighlightFile(activeWin); */
/*     for (int i = 0; i < COUNT_SUBWINS; i++) */
/*     { */
/*         int width = getmaxx(activeWin->subWins[i]) - 2; */
/*         mvwchgat(activeWin->subWins[i], y, 1, width, A_REVERSE, 1, NULL); */
/*         wrefresh(activeWin->subWins[i]); */
/*     } */
/*     return 0; */
/* } */
void refreshChat(Chat* win)
{
    wrefresh(win->msgWin);
    wrefresh(win->nicknameWin);
    wrefresh(win->inputWin);
    refresh();
}
/* void clearMyWin(Chat* myWin) */
/* { */
/*     int start_line = 2; */
/*     for (int i = start_line; i < LINES; i++) */
/*     { */
/*         wmove(myWin->subWins[0], i, 1); */
/*         wclrtoeol(myWin->subWins[0]); */

/*         wmove(myWin->subWins[1], i, 1); */
/*         whline(myWin->subWins[1], ' ', getmaxx(myWin->subWins[1])); */

/*         wmove(myWin->subWins[2], i, 0); */
/*         wclrtoeol(myWin->subWins[2]); */
/*     } */
/*     box(myWin->win, 0, 0); */
/*     wborder(myWin->subWins[1], ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_TTEE, ACS_TTEE, ACS_BTEE, ACS_BTEE); */
/*     refreshChat(myWin); */
/* } */
void destroyChat(Chat* chat)
{
    if (!chat) return;

    delwin(chat->msgWin);
    delwin(chat->inputWin);
    delwin(chat->nicknameWin);

    free(chat);
}
int main()
{
  initChat();
  Chat *chat = createChat();
  getch();
  destroyChat(chat);

  return 0;
}
