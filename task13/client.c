#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <mqueue.h>

#include <unistd.h>

#define MAX_COUNT_MESSAGES 8
#define MAX_MSG_SIZE 20

#define MSG_PRIO 1
#define NICKNAME_PRIO 2
#define MAX_LENGTH_MSG 256
#define MAX_LENGTH_NICKNAME 20
#define MAX_COUNT_MSGS 50
#define MAX_COUNT_NICKNAMES 16

int main()
{
    char nicknames[MAX_COUNT_NICKNAMES][MAX_LENGTH_NICKNAME];
    char msgs[MAX_COUNT_MSGS][MAX_LENGTH_MSG];

    char nickname[MAX_LENGTH_NICKNAME];
    char bufNick[MAX_LENGTH_NICKNAME+1] = "/";
    char msg[MAX_LENGTH_MSG];


    struct mq_attr attr;
    attr.mq_maxmsg = MAX_COUNT_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;

    mqd_t serviceServerQueue = mq_open("/serviceServerQueue", O_WRONLY | O_CREAT, 0600, &attr);
    if (serviceServerQueue == -1)
    {
        perror("Failed create queue");
        exit(EXIT_FAILURE);
    }

    printf("Enter nickname\n");
    fgets(nickname, MAX_LENGTH_NICKNAME, stdin);
    strcat(bufNick, nickname);
    printf("%s\n", bufNick);
    mqd_t serviceClientQueue = mq_open(bufNick, O_RDONLY | O_CREAT, 0600, &attr);
    if (serviceClientQueue == -1)
    {
        perror("Failed create queue");
        exit(EXIT_FAILURE);
    }

    mq_send(serviceServerQueue, nickname, strlen(nickname) + 1, NICKNAME_PRIO);

    printf("Press any button");
getchar();

      if(mq_close(serviceClientQueue) == -1)
      {
        perror("Failed close");
        exit(1);
      }
      if(mq_unlink(bufNick) == -1)
      {
        perror("Failed unlink");
        exit(1);
      }

    return 0;
}
