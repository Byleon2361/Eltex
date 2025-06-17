#include <stdio.h>
#include <pthread.h>
#define N 5
void *tread_calc(void *args)
{
  int *i = (int*) args;
  printf("thread num %d\n", *i);

  return NULL;
}
int main()
{
  int *s;
  int v[N];
  pthread_t thread[N];

  for(int i = 0; i < N; i++)
  {
    v[i] = i;
    pthread_create(&thread[i], NULL, tread_calc, (void *)&v[i]);
  }
  for(int i = 0; i < N; i++)
  {
    pthread_join(thread[i], (void**) &s);
  }
  return 0;
}
