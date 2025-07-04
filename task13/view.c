#include "view.h"
SCREEN* s = NULL;
void initChat()
{
  s = newterm(NULL, stdout, stdin);

  clear();
  noecho();
  cbreak();
  notimeout(stdscr, 0);

  curs_set(0);

  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_BLACK, COLOR_CYAN);
}
Chat* createChat()
{
  Chat* chat = malloc(sizeof(Chat));

  WINDOW* msgWin = newwin(LINES - 3, COLS * 4 / 6, 0, 0);
  WINDOW* nicknameWin = newwin(LINES - 3, COLS * 2 / 6, 0, COLS * 4 / 6);
  WINDOW* inputWin = newwin(3, COLS, LINES - 3, 0);

  wbkgd(msgWin, COLOR_PAIR(1));
  wbkgd(nicknameWin, COLOR_PAIR(1));
  wbkgd(inputWin, COLOR_PAIR(1));

  scrollok(nicknameWin, 1);
  scrollok(msgWin, 1);

  wborder(msgWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wborder(nicknameWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wborder(inputWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

  chat->msgWin = msgWin;
  chat->nicknameWin = nicknameWin;
  chat->inputWin = inputWin;

  refreshChat(chat);

  return chat;
}
void refreshChat(Chat* chat)
{
  wborder(chat->msgWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wborder(chat->nicknameWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wborder(chat->inputWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

  wrefresh(chat->msgWin);
  wrefresh(chat->nicknameWin);
  wrefresh(chat->inputWin);
  refresh();
}
void destroyChat(Chat* chat)
{
  if (!chat) return;

  delwin(chat->msgWin);
  delwin(chat->inputWin);
  delwin(chat->nicknameWin);
  free(chat);

  endwin();
  delscreen(s);
}
void printMsg(Chat* chat, char* msg)
{
  int maxY = getmaxy(chat->msgWin)-2;
  int x = 0;
  int y = 0;

  getyx(chat->msgWin, y, x);
  if(y >= maxY)
  {
    wscrl(chat->msgWin,1);
    y = maxY;
    waddstr(chat->msgWin, "\n");
  }
  else
  {
    y++;
  }
  wmove(chat->msgWin, y, 1);

  waddstr(chat->msgWin, msg);

  wborder(chat->msgWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wrefresh(chat->msgWin);
}
void clearMsgWin(Chat* chat)
{
  wclear(chat->msgWin);
  wborder(chat->msgWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wrefresh(chat->msgWin);
}
void printNickname(Chat* chat, char* nickname, char* currentNickname)
{
  int maxY = getmaxy(chat->msgWin)-2;
  int x = 0;
  int y = 0;

  getyx(chat->nicknameWin, y, x);
  if(y >= maxY)
  {
    wscrl(chat->nicknameWin,1);
    y = maxY;
    waddstr(chat->msgWin, "\n");
  }
  else
  {
    y++;
  }
  wmove(chat->nicknameWin, y, 1);
  waddstr(chat->nicknameWin, nickname);
  if (strcmp(nickname, currentNickname) == 0)
  {
    int width = getmaxx(chat->nicknameWin) - 2;
    mvwchgat(chat->nicknameWin, y, 1, width, A_REVERSE, 1, NULL);
  }

  wborder(chat->nicknameWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wrefresh(chat->nicknameWin);
}
void clearNicknameWin(Chat* chat)
{
  wclear(chat->nicknameWin);
  wborder(chat->nicknameWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wrefresh(chat->nicknameWin);
}
void enterMsg(Chat* chat, char* msg, int maxLengthMsg)
{
  echo();
  mvwgetnstr(chat->inputWin, 1, 1, msg, maxLengthMsg);
  noecho();
  wclear(chat->inputWin);
  wborder(chat->inputWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wrefresh(chat->inputWin);
}
/* int main() */
/* { */
/*   initChat(); */
/*   Chat *chat = createChat(); */

/*   clearMsgWin(chat); */
/*   clearNicknameWin(chat); */

/*   printNickname(chat, "asd", "asd"); */
/*   printNickname(chat, "qwe", "asd"); */
/*   printNickname(chat, "zxc", "asd"); */

/*   printMsg(chat, "Hello, world"); */
/*   printMsg(chat, "Hi"); */
/*   printMsg(chat, "How are you?"); */
/*   printMsg(chat, "I am okay"); */

/*   char buf[20]; */
/*   while(1) */
/*   { */
/*    enterMsg(chat, buf, 20);  */
/*     if(strcmp(buf, "exit") == 0) */
/*     { */
/*       break; */
/*     } */
/*    printMsg(chat, buf); */
/*   } */
/*   destroyChat(chat); */

/*   return 0; */
/* } */
