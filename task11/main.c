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
pthread_mutex_t shopMutexes[COUNT_SHOPS] = PTHREAD_MUTEX_INITIALIZER;
int customerShopPtr = 0;
int loaderShopPtr = 0;
int stopLoader = 0;
pthread_mutex_t stopLoaderMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t loaderPtrMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t customerPtrMutex = PTHREAD_MUTEX_INITIALIZER;

void *customerFunc(void *args)
{
  int* needPtr = (int *) args;
  int need = *needPtr;
  int took=0;
  int currentShop;
  while(need > 0)
  {
    pthread_mutex_lock(&customerPtrMutex);
    currentShop = customerShopPtr;
    if(++customerShopPtr >= COUNT_SHOPS)customerShopPtr = 0;
    pthread_mutex_unlock(&customerPtrMutex);

    pthread_mutex_lock(&shopMutexes[currentShop]);
    fprintf(stdout, "Customer: %ld, need before: %d, ", (long)pthread_self(), need);
    fprintf(stdout, "shop: %d, products before: %d, ", currentShop, shops[currentShop]); 

    if(need >= shops[currentShop]) {
      took = shops[currentShop];
      need -= shops[currentShop];
      shops[currentShop] = 0;
    }
    else
    {
      took = need;
      shops[currentShop] -= need;
      need = 0;
    }

    fprintf(stdout, "took: %d, need after: %d, products after: %d, sleep: %d sec\n\n", took, need, shops[currentShop], SLEEP_TIME_CUSTOMER);

    pthread_mutex_unlock(&shopMutexes[currentShop]);
    sleep(SLEEP_TIME_CUSTOMER);
  }

  return NULL;
}
void *loaderFunc(void *args)
{
  int currentShop;
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

    pthread_mutex_lock(&loaderPtrMutex);
    currentShop = loaderShopPtr;
    if(++loaderShopPtr >= COUNT_SHOPS) loaderShopPtr = 0;
    pthread_mutex_unlock(&loaderPtrMutex);

    pthread_mutex_lock(&shopMutexes[currentShop]);
    fprintf(stdout, "Loader %ld, ", (long)pthread_self());
    fprintf(stdout, "shop %d, products before: %d, ", currentShop, shops[currentShop]); 

    if((shops[currentShop]+ COUNT_NEWPRODUCTS) < MAX_COUNT_PRODUCTS)
    {
      put = COUNT_NEWPRODUCTS;
      shops[currentShop] += COUNT_NEWPRODUCTS;
    }
    else
    {
      put = MAX_COUNT_PRODUCTS - shops[currentShop];
      shops[currentShop] = MAX_COUNT_PRODUCTS;
    }

    fprintf(stdout, "put: %d, products after: %d, sleep: %d sec\n\n", put, shops[currentShop], SLEEP_TIME_LOADER);

    pthread_mutex_unlock(&shopMutexes[currentShop]);
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
