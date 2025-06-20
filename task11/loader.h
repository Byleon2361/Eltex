#ifndef LOADER_H
#define LOADER_H
#include "common.h"

#define COUNT_LOADER_THREADS 1
#define MAX_COUNT_PRODUCTS 10000
#define COUNT_NEWPRODUCTS 5000
#define SLEEP_TIME_LOADER 1

void *loaderFunc(void *args);
#endif
