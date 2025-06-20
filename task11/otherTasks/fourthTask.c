#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <malloc.h>

#define N 5

pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_key_t key;

void dest(void *buff)
{
  free(buff);
}
void key_create()
{
  pthread_key_create(&key, dest);
}
char *error_str(int no)
{
  char *buff;

  pthread_once(&once, key_create);

  buff = pthread_getspecific(key);
  if(buff == NULL)
  {
    buff = malloc(255);
    pthread_setspecific(key, buff);
  }

  switch(no)
  {
    case 1:
      strcpy(buff, "Error 1");
      break;
    case 2:
      strcpy(buff, "Error 2");
      break;
  }

  return buff;
}
void *tread_calc(void *args)
{
  int *i = (int*) args;
  int err = (*i%2==0)?2:1;

  printf("%s\n", error_str(err));

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
