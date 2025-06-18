#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define COUNT_CUSTOMER_THREADS 3
#define COUNT_LOADER_THREADS 1
#define COUNT_SHOPS 5
#define MAX_COUNT_PRODUCTS 10000
#define MAX_COUNT_NEEDS 100000
#define COUNT_NEWPRODUCTS 5000
#define SLEEP_TIME_CUSTOMER 2
#define SLEEP_TIME_LOADER 1

int shops[COUNT_SHOPS];
FILE* outputFile; 
pthread_mutex_t customerShopMutexes[COUNT_SHOPS] = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t loaderShopMutexes[COUNT_SHOPS] = PTHREAD_MUTEX_INITIALIZER;
int customerShopPtr = 0;
int loaderShopPtr = 0;
int stopLoader = 0;
pthread_mutex_t stopLoaderMutex = PTHREAD_MUTEX_INITIALIZER;
void *customerFunc(void *args)
{
  int* needPtr = (int *) args;
  int need = *needPtr;
  int took=0;
  while(need > 0)
  {

    pthread_mutex_lock(&customerShopMutexes[customerShopPtr]);
    if(customerShopPtr < COUNT_SHOPS) customerShopPtr++;
    else customerShopPtr = 0;

    fprintf(stdout, "I am customers %ld, i need %d, ", (long)pthread_self(), need);
    fprintf(stdout, "i came into the shop %d, there was %d products, ", customerShopPtr, shops[customerShopPtr]); 

    if(need >= shops[customerShopPtr]) {
      took = shops[customerShopPtr];
      need -= shops[customerShopPtr];
      shops[customerShopPtr] = 0;
    }
    else
    {
      took = need;
      shops[customerShopPtr] -= need;
      need = 0;
    }

    fprintf(stdout, "i took %d products, my need has become %d, shops products have become %d, i fell asleep for %d seconds\n\n", took, need, shops[customerShopPtr], SLEEP_TIME_CUSTOMER);

    pthread_mutex_unlock(&customerShopMutexes[customerShopPtr]);
    sleep(SLEEP_TIME_CUSTOMER);
  }

  return NULL;
}
void *loaderFunc(void *args)
{
  int put=0;
  while(1)
  {
    pthread_mutex_lock(&stopLoaderMutex);
    if(stopLoader == 1)
    {
      pthread_mutex_unlock(&stopLoaderMutex);
      break;
    }
      pthread_mutex_unlock(&stopLoaderMutex);

    pthread_mutex_lock(&loaderShopMutexes[loaderShopPtr]);
    if(loaderShopPtr < COUNT_SHOPS) loaderShopPtr++;
    else loaderShopPtr = 0;

    fprintf(stdout, "I am loader %ld, ", (long)pthread_self());
    fprintf(stdout, "i came into the shop %d, there was %d products, ", loaderShopPtr, shops[loaderShopPtr]); 

    if((shops[loaderShopPtr]+ COUNT_NEWPRODUCTS) < MAX_COUNT_PRODUCTS)
    {
      put = COUNT_NEWPRODUCTS;
      shops[loaderShopPtr] += COUNT_NEWPRODUCTS;
    }
    else
    {
      put = MAX_COUNT_PRODUCTS - shops[loaderShopPtr];
      shops[loaderShopPtr] = MAX_COUNT_PRODUCTS;
    }

    fprintf(stdout, "i put %d products, shops products have become %d, i fell asleep for %d seconds\n\n", put, shops[loaderShopPtr], SLEEP_TIME_LOADER);

    pthread_mutex_unlock(&loaderShopMutexes[loaderShopPtr]);
    sleep(SLEEP_TIME_LOADER);
  }

  return NULL;
}
int main()
{
  int customersNeeds[COUNT_CUSTOMER_THREADS];
  pthread_t customers[COUNT_CUSTOMER_THREADS];
  pthread_t loaders[COUNT_LOADER_THREADS];

  srand(time(NULL));

  outputFile = fopen("output.log", "w");
  if(!outputFile)
  {
    fprintf(stderr, "Failed open file\n");
    return 1;
  }

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
  for(int i = 0; i < COUNT_LOADER_THREADS; i++)
  {
    pthread_create(&loaders[i], NULL, loaderFunc,NULL);
  }

  for(int i = 0; i < COUNT_CUSTOMER_THREADS; i++)
  {
    pthread_join(customers[i], NULL);
  printf("------------------------------");
  }

  pthread_mutex_lock(&stopLoaderMutex);
  stopLoader = 1;
  pthread_mutex_unlock(&stopLoaderMutex);

  for(int i = 0; i < COUNT_LOADER_THREADS; i++)
  {
    pthread_join(loaders[i], NULL);
  }
  fclose(outputFile);
  return 0;
}
