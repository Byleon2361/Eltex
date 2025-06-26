#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <mqueue.h>
#define MAX_LENGTH_TEXT 20
#define MAX_COUNT_MESSAGES 8
#define MAX_MSG_SIZE 20

int main()
{
    char msgSnd[MAX_LENGTH_TEXT] = "Hello";
    char msgRcv[MAX_LENGTH_TEXT];

    mqd_t idServer = mq_open("/toServer", O_RDWR);
    mqd_t idClient = mq_open("/toClient", O_RDWR);

    mq_receive(idClient, msgRcv, sizeof(msgRcv), NULL);
    printf("%s\n", (char*)msgRcv);

    int prio = 2;
    mq_send(idServer, msgSnd, strlen(msgSnd) + 1, prio);

    mq_close(idServer);
    mq_close(idClient);

    return 0;
}
