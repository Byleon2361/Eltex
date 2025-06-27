#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

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
  struct client
  {
    mqd_t id;
    char nickname[MAX_LENGTH_NICKNAME];
  } clients[MAX_COUNT_NICKNAMES];

    char msgs[MAX_COUNT_MSGS][MAX_LENGTH_MSG];
    /* char bufNick[MAX_LENGTH_NICKNAME+1] = "/"; */
    char *bufNick = malloc(sizeof(char) * (MAX_LENGTH_NICKNAME+1));

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_COUNT_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;

    mqd_t serviceServerQueue = mq_open("/serviceServerQueue", O_RDONLY | O_CREAT, 0600, &attr);
    if (serviceServerQueue == -1)
    {
        perror("Failed create queue");
        exit(EXIT_FAILURE);
    }

    char in[MAX_LENGTH_MSG];
    char out[MAX_LENGTH_MSG];
    int i = 0;
    while(1)
    {
      if(mq_receive(serviceServerQueue, in, sizeof(in), NULL) == -1)
      {
        perror("Failed serviceServerQueue receive");
        exit(1);
      }

      memset(bufNick, 0, MAX_LENGTH_NICKNAME);
      bufNick[0] = '\\';
      strncpy(clients[i].nickname, in, sizeof(clients[0].nickname));
      strcat(bufNick, in);
      printf("%s\n", bufNick);
      clients[i].id = mq_open(bufNick, O_WRONLY);
      if(clients[i].id == -1)
      {
        perror("Failed open client");
        exit(1);
      }
      printf("%s\n", clients[i].nickname);

      /* for(int j = 0; j < i; j++) */
      /* { */
      /*   if(mq_send(clients[i].id, clients[j].nickname, strlen(clients[j].nickname) + 1, NICKNAME_PRIO) == -1) */
      /*   { */
      /*     perror("Failed client send"); */
      /*     exit(1); */
      /*   } */
      /* } */
      i++;
    }

      if(mq_close(serviceServerQueue) == -1)
      {
        perror("Failed close");
        exit(1);
      }
      if(mq_unlink("/serviceServerQueue") == -1)
      {
        perror("Failed unlink");
        exit(1);
      }

    return 0;
}
