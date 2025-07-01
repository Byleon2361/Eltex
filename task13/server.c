#include "manager.h"
#include <pthread.h>
int main()
{
  pthread_t nicknameThread;
  pthread_t msgThread;

  pthread_create(&nicknameThread, NULL, nicknameMain, NULL);
  pthread_create(&msgThread, NULL, msgMain, NULL);

  pthread_join(nicknameThread,NULL);
  pthread_join(msgThread,NULL);

  return 0;
}
