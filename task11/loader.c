#include "loader.h"

extern int loaderShopPtr;
extern int stopLoader;
extern pthread_mutex_t stopLoaderMutex;
extern pthread_mutex_t loaderPtrMutex;
extern int shops[COUNT_SHOPS];
extern FILE* outputFile;
extern pthread_mutex_t shopMutexes[COUNT_SHOPS];
extern pthread_mutex_t fileMutex;

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
      if(shops[currentShop]<MAX_COUNT_PRODUCTS)
      {
        pthread_mutex_lock(&fileMutex);
        fprintf(outputFile, "Loader %ld, ", (long)pthread_self());
        fprintf(outputFile, "shop %d, products before: %d, ", currentShop, shops[currentShop]); 
        pthread_mutex_unlock(&fileMutex);

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

        pthread_mutex_lock(&fileMutex);
        fprintf(outputFile, "put: %d, products after: %d, sleep: %d sec\n\n", put, shops[currentShop], SLEEP_TIME_LOADER);
        pthread_mutex_unlock(&fileMutex);

        pthread_mutex_unlock(&shopMutexes[currentShop]);
      }
      else
      {
        pthread_mutex_lock(&fileMutex);
        fprintf(outputFile, "Loader: %ld, shop: %d, shop is full\n\n", (long)pthread_self(), MAX_COUNT_PRODUCTS);
        pthread_mutex_unlock(&fileMutex);

        pthread_mutex_unlock(&shopMutexes[currentShop]);
      }
    sleep(SLEEP_TIME_LOADER);
  }

  return NULL;
}
