#include <stdlib.h>
#include "customer.h"
#include "loader.h"

int shops[COUNT_SHOPS] = {0};
FILE* outputFile = NULL;
pthread_mutex_t shopMutexes[COUNT_SHOPS] = PTHREAD_MUTEX_INITIALIZER;
int customerShopPtr = 0;
int loaderShopPtr = 0;
int stopLoader = 0;
pthread_mutex_t stopLoaderMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t loaderPtrMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t customerPtrMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    int s = 0;
    int customersNeeds[COUNT_CUSTOMER_THREADS];
    pthread_t customers[COUNT_CUSTOMER_THREADS];
    pthread_t loaders[COUNT_LOADER_THREADS];

    srand(time(NULL));

    outputFile = fopen("output.log", "w");
    if (!outputFile)
    {
        fprintf(stderr, "Failed open file\n");
        return 1;
    }

    for (int i = 0; i < COUNT_SHOPS; i++)
    {
        shops[i] = (rand() % (MAX_COUNT_PRODUCTS-1))+1;
    }
    for (int i = 0; i < COUNT_CUSTOMER_THREADS; i++)
    {
        customersNeeds[i] = (rand() % (MAX_COUNT_NEEDS-1))+1;
    }

    for (int i = 0; i < COUNT_CUSTOMER_THREADS; i++)
    {
        s = pthread_create(&customers[i], NULL, customerFunc, (void*)&customersNeeds[i]);
        if (s != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < COUNT_LOADER_THREADS; i++)
    {
        s = pthread_create(&loaders[i], NULL, loaderFunc, NULL);
        if (s != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < COUNT_CUSTOMER_THREADS; i++)
    {
        pthread_join(customers[i], NULL);
        if (s != 0)
        {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_lock(&stopLoaderMutex);
    stopLoader = 1;
    pthread_mutex_unlock(&stopLoaderMutex);

    for (int i = 0; i < COUNT_LOADER_THREADS; i++)
    {
        pthread_join(loaders[i], NULL);
        if (s != 0)
        {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }
    fclose(outputFile);
    return 0;
}
