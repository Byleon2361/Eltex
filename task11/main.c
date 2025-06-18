#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define COUNT_CUSTOMER_THREADS 3
#define COUNT_LOADER_THREADS 1
#define COUNT_SHOPS 5
#define MAX_COUNT_PRODUCTS 10000
#define MAX_COUNT_NEEDS 1000
#define MAX_COUNT_NEWPRODUCTS 5000
#define SLEEP_TIME_CUSTOMER 2
#define SLEEP_TIME_LOADER

/* void *tread_calc(void *args) */
/* { */
/*   int* aThread = (int *) args; */

/*   for(int i=0; i < 10000000; i++) */
/*   { */
/*     (*aThread)++; */
/*   } */
/*   return NULL; */
/* } */

int shops[COUNT_SHOPS];
FILE* outputFile; 
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
int shopPtr = 0;
void *customerFunc(void *args)
{
  int* needPtr = (int *) args;
  int need = *needPtr;
  int took=0;
  while(need > 0)
  {
    pthread_mutex_lock(&m1);

fprintf(outputFile, "I am customers %ld, i need %d, ", (long)pthread_self(), need);

    if(shopPtr < COUNT_SHOPS) shopPtr++;
    else shopPtr = 0;

    fprintf(outputFile, "i came into the shop %d, there was %d products, ", shopPtr, shops[shopPtr]); 
    if(need >= shops[shopPtr]) {
      took = shops[shopPtr];
      need -= shops[shopPtr];
      shops[shopPtr] = 0;
    }
    else
    {
      took = need;
      shops[shopPtr] -= need;
      need = 0;
    }

    
    fprintf(outputFile, "i took %d products, my need has become %d, shops products have become %d\n ", took, need, shops[shopPtr]);

    pthread_mutex_unlock(&m1);
    sleep(SLEEP_TIME_CUSTOMER);
  }



return NULL;
}
/* void *loaderFunc(void *args) */
/* { */
/* } */
int main()
{
  int *s;
  int customersNeeds[COUNT_CUSTOMER_THREADS];
  pthread_t customers[COUNT_CUSTOMER_THREADS];
  pthread_t loaders[COUNT_LOADER_THREADS];

  srand(time(NULL));

   outputFile = fopen("output.log", "w");

  for(int i = 0; i < COUNT_SHOPS; i++)
  {
    shops[i] = rand() % MAX_COUNT_PRODUCTS;
  }
  for(int i = 0; i < COUNT_CUSTOMER_THREADS; i++)
  {
    customersNeeds[i] = rand() % MAX_COUNT_NEEDS;
  }

  for(int i = 0; i < COUNT_CUSTOMER_THREADS; i++)
  {
    pthread_create(&customers[i], NULL, customerFunc,(void*)&customersNeeds[i]);
  }

  /* for(int i = 0; i < COUNT_LOADER_THREADS; i++) */
  /* { */
  /*   pthread_create(&loaders[i], NULL, loaderFunc,(void*)&allA[i]); */
  /* } */
  
  for(int i = 0; i < COUNT_CUSTOMER_THREADS; i++)
  {
    pthread_join(customers[i], (void**) &s);
  }

  /* for(int i = 0; i < COUNT_LOADER_THREADS; i++) */
  /* { */
  /*   pthread_join(loaders[i], (void**) &s); */
  /* } */
fclose(outputFile);
  return 0;
}


