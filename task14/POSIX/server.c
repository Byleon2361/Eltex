#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define MAX_LENGTH_TEXT 20
#define MAX_COUNT_MESSAGES 8
#define MAX_MSG_SIZE 20

int main()
{
    char msgSnd[MAX_LENGTH_TEXT] = "Hi";
    char msgRcv[MAX_LENGTH_TEXT];

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_COUNT_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;

    mqd_t idServer = mq_open("/toServer", O_RDWR | O_CREAT, 0600, &attr);
    if (idServer == -1)
    {
        perror("Failed create queue");
        exit(EXIT_FAILURE);
    }
    mqd_t idClient = mq_open("/toClient", O_RDWR | O_CREAT, 0600, &attr);
    if (idServer == -1)
    {
        perror("Failed create queue");
        exit(EXIT_FAILURE);
    }

    int prio = 1;
    mq_send(idClient, msgSnd, strlen(msgSnd) + 1, prio);

    mq_receive(idServer, msgRcv, sizeof(msgRcv), NULL);

    printf("%s\n", (char*)msgRcv);

    mq_close(idServer);
    mq_close(idClient);
    mq_unlink("/toServer");
    mq_unlink("/toClient");

    return 0;
}
