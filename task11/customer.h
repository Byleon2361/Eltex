#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "common.h"

#define COUNT_CUSTOMER_THREADS 3
#define SLEEP_TIME_CUSTOMER 2
#define MAX_COUNT_NEEDS 100000

void *customerFunc(void *args);

#endif
