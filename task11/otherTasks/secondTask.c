#include <stdio.h>
#include <pthread.h>

#define N 16

long a = 0;

void *tread_calc(void *args)
{
  int* aThread = (int *) args;

  for(int i=0; i < 10000000; i++)
  {
    (*aThread)++;
  }
  return NULL;
}
int main()
{
  int *s;
  int allA[N] = {0};
  pthread_t thread[N];

  for(int i = 0; i < N; i++)
  {
    pthread_create(&thread[i], NULL, tread_calc,(void*)&allA[i]);
  }

  for(int i = 0; i < N; i++)
  {
    pthread_join(thread[i], (void**) &s);
    a += allA[i];
  }

  printf("a = %ld\n", a);
  return 0;
}

