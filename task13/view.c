#include "view.h"
SCREEN* s = NULL;
void initChat()
{
    s = newterm(NULL,stdout, stdin);

    cbreak();
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
  int x = 0;
  int y = 0;

  getyx(chat->msgWin,y,x);
  y++;
  wmove(chat->msgWin,y,1); 

  waddstr(chat->msgWin, msg);
  refreshChat(chat);
}
void printNicknames(Chat* chat, char **nicknames, int countNicknames, char* currentNickname)
{
  int x = 0;
  int y = 0;
  getyx(chat->nicknameWin,y,x);
  for(int i = 0; i < countNicknames; i++)
  {
    y++;
    wmove(chat->nicknameWin,y,1); 
    waddstr(chat->nicknameWin, nicknames[i]);
    if(strcmp(nicknames[i], currentNickname) == 0)
    {
        int width = getmaxx(chat->nicknameWin) - 2;
        mvwchgat(chat->nicknameWin, y, 1, width, A_REVERSE, 1, NULL);
    }
  }
  refreshChat(chat);
}
char* enterMsg(Chat* chat, char* msg, int maxLengthMsg)
{
  mvwgetnstr(chat->inputWin,1,1, msg, maxLengthMsg);
  wclear(chat->inputWin);

  return NULL;
}
int main()
{
  char *nicknames[3] = {"qwe\n", "asd\n", "zxc\n"};
  initChat();
  Chat *chat = createChat();
  printNicknames(chat, nicknames, 3, "asd\n");
  printMsg(chat, "Hello, world");
  printMsg(chat, "Hi");
  printMsg(chat, "How are you?");
  printMsg(chat, "I am okay");
  char buf[20];
  while(1)
  {
   enterMsg(chat, buf, 20); 
    if(strcmp(buf, "exit") == 0)
    {
      break;
    }
   printMsg(chat, buf);
  }
  destroyChat(chat);

  return 0;
}
