#include "customer.h"

extern int shops[COUNT_SHOPS];
extern FILE* outputFile;
extern pthread_mutex_t shopMutexes[COUNT_SHOPS];
extern int customerShopPtr;
extern pthread_mutex_t customerPtrMutex;
extern pthread_mutex_t fileMutex;

void* customerFunc(void* args)
{
    int* needPtr = (int*)args;
    int need = *needPtr;
    int took = 0;
    int currentShop;
    while (need > 0)
    {
        pthread_mutex_lock(&customerPtrMutex);
        currentShop = customerShopPtr;
        if (++customerShopPtr >= COUNT_SHOPS) customerShopPtr = 0;
        pthread_mutex_unlock(&customerPtrMutex);

        pthread_mutex_lock(&shopMutexes[currentShop]);
        if (shops[currentShop] > 0)
        {
        pthread_mutex_lock(&fileMutex);
            fprintf(outputFile, "Customer: %ld, need before: %d, ", (long)pthread_self(), need);
            fprintf(outputFile, "shop: %d, products before: %d, ", currentShop, shops[currentShop]);
        pthread_mutex_unlock(&fileMutex);

            if (need >= shops[currentShop])
            {
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

        pthread_mutex_lock(&fileMutex);
            fprintf(outputFile, "took: %d, need after: %d, products after: %d, sleep: %d sec\n\n", took, need, shops[currentShop], SLEEP_TIME_CUSTOMER);
        pthread_mutex_unlock(&fileMutex);

            pthread_mutex_unlock(&shopMutexes[currentShop]);
        }
        else
        {
        pthread_mutex_lock(&fileMutex);
            fprintf(outputFile, "Customer: %ld, shop: %d, shop is empty\n\n", (long)pthread_self(), currentShop);
        pthread_mutex_unlock(&fileMutex);

            pthread_mutex_unlock(&shopMutexes[currentShop]);
        }
        sleep(SLEEP_TIME_CUSTOMER);
    }

    return NULL;
}
