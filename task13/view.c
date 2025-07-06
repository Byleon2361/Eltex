#include "view.h"
#include <ctype.h>

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
void enterMsg(Chat* chat, char* msg, int maxLengthMsg)
{
  int maxY = getmaxy(chat->inputWin)-2;
  int maxX = getmaxx(chat->inputWin)-2;
  int x = 0;
  int y = 0;
  char ch;
  int lineCount = 0;
  int symCount = 0;
  int allCount = 0;

  wmove(chat->inputWin, 1, 1);
  scrollok(chat->inputWin, 1);

  while(((ch = wgetch(chat->inputWin)) != '\n') && (allCount < maxLengthMsg-1))
  {
    if(ch == 127)
    {
      if(lineCount <= 0 && symCount <= 0)
      {
        printMsg(chat, "NULL all wewe");
        lineCount = 0;
        symCount = 0;
        continue;
      }
      wmove(chat->inputWin, 1, symCount);
      wdelch(chat->inputWin);
      winsch(chat->inputWin, '\n');


      allCount = maxX*lineCount+symCount;
      msg[allCount] = '\0';

      symCount--;

      if(symCount == 0 && lineCount >0)
      {
        lineCount--;
        symCount = maxX;
        mvwaddnstr(chat->inputWin, 1, 1, msg+(lineCount*maxX), maxX);
        waddstr(chat->inputWin, "\n");
      }
        wborder(chat->inputWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    }
    else if(isprint(ch))
    {
      if(symCount >= maxX)
      {
        lineCount++;
        symCount = 0;
        wmove(chat->inputWin, 1, 1);
        waddstr(chat->inputWin, "\n");
        wborder(chat->inputWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
        wmove(chat->inputWin, 1, 1);

      }
      waddch(chat->inputWin, ch);

      allCount = maxX*lineCount+symCount;
      msg[allCount] = ch;

      symCount++;
    }
  }
  allCount = maxX*lineCount+symCount;
  msg[allCount] = '\0';

  wclear(chat->inputWin);
  wborder(chat->inputWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wrefresh(chat->inputWin);
}
void printMsg(Chat* chat, char* msg)
{
  int maxY = getmaxy(chat->msgWin)-2;
  int maxX = getmaxx(chat->msgWin)-2;
  int x = 0;
  int y = 0;

  int countOutputSymbols = 0;
  int count = 0;
  char* partOfMsg = malloc(sizeof(char)*(maxX+1));
  int msgLength = strlen(msg);
  while(countOutputSymbols < msgLength)
  {
    getyx(chat->msgWin, y, x);
    if(y >= maxY)
    {
      wscrl(chat->msgWin,1);
      y = maxY;
    }
    else
    {
      y++;
    }
    wmove(chat->msgWin, y, 1);

    if(msgLength - countOutputSymbols < maxX)
    {
      count = msgLength - countOutputSymbols;
    }
    else
    {
      count = maxX;
    }
    strncpy(partOfMsg, msg, count);
    partOfMsg[count] = '\0';

    countOutputSymbols += count;
    msg += count;
    waddstr(chat->msgWin, partOfMsg);
  }
      wclrtoeol(chat->msgWin);
      wborder(chat->msgWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  free(partOfMsg);
  wrefresh(chat->msgWin);
}
void printNickname(Chat* chat, char* nickname, char* currentNickname)
{
  int maxY = getmaxy(chat->nicknameWin)-2;
  int maxX = getmaxx(chat->nicknameWin)-2;
  int x = 0;
  int y = 0;

  int countOutputSymbols = 0;
  char* partOfNickname = malloc(sizeof(char)*(maxX+1));
  int nicknameLength = strlen(nickname);
  int isCurrent = 0;
  int count = 0;
  int countLines= 0;

  if (strcmp(nickname, currentNickname) == 0)
  {
    isCurrent = 1;
  }

  while(countOutputSymbols < nicknameLength)
  {
    getyx(chat->nicknameWin, y, x);
    if(y >= maxY)
    {
      wscrl(chat->nicknameWin,1);
      y = maxY;
    }
    else
    {
      y++;
    }
    wmove(chat->nicknameWin, y, 1);

    if(nicknameLength - countOutputSymbols < maxX)
    {
      count = nicknameLength - countOutputSymbols;
    }
    else
    {
      count = maxX;
    }
    strncpy(partOfNickname, nickname, count);
    partOfNickname[count] = '\0';

    countOutputSymbols += count;
    nickname += count;
    waddstr(chat->nicknameWin, partOfNickname);
      wclrtoeol(chat->nicknameWin);
if(isCurrent)
{
      mvwchgat(chat->nicknameWin, y, 1, maxX, A_REVERSE, 1, NULL);
}
    countLines++;
  }
      wborder(chat->nicknameWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  free(partOfNickname);
  wrefresh(chat->nicknameWin);
}
void clearNicknameWin(Chat* chat)
{
  wclear(chat->nicknameWin);
  wborder(chat->nicknameWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wrefresh(chat->nicknameWin);
}
void clearMsgWin(Chat* chat)
{
  wclear(chat->msgWin);
  wborder(chat->msgWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
  wrefresh(chat->msgWin);
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
