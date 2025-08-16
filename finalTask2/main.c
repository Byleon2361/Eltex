#include "driver.h"
void printMenu()
{
  printf("1) Создать водителя\n");
  printf("2) Создать задачу\n");
  printf("3) Получить статус\n");
  printf("4) Вывести информацию о всех водителей\n");
  printf("5) Выйти\n");
}
int main()
{
  char choice;
  int exit = 0;
  pid_t pid;
  int sec = 0;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  init();

  do
  {
    printMenu();
    choice = getchar();
    switch(choice)
    {
      case '1':
        create_driver();
        break;
      case '2':
        printf("Введите pid процесса\n");
        scanf("%10d", &pid);
        printf("Введите время в течение которого процесс будет выполняться(в секундах)\n");
        scanf("%10d", &sec);
        tv.tv_sec = sec;
        send_task(pid, tv);
        break;
      case '3':
        printf("Введите pid процесса\n");
        scanf("%10d", &pid);
        enum status status = get_status(pid);
        printf("-----------------------\n");

        if(status == AVAILABLE)
          printf("Процесс %d - AVAILABLE\n", pid);
        else if(status == BUSY)
          printf("Процесс %d - BUSY\n", pid);
        else
          printf("Процесса с pid=%d не существует\n", pid);

        printf("status %d\n", status);
        break;
      case '4':
        get_drivers();
        break;
      case '5':
        exit = 1;
        break;
      default:
        printf("Такого пункта меню нет\n");
        break;
    }
  }
  while(exit != 1);
  return 0;
}
