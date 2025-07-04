#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>

#include "view.h"

#define MAX_COUNT_MESSAGES 8
#define MAX_MSG_SIZE 20

#define MSG_PRIO 1
#define NICKNAME_PRIO 2
#define CLEAR_PRIO 3
#define DIED_PRIO 4
#define MAX_LENGTH_MSG 256
#define MAX_LENGTH_NICKNAME 20
#define MAX_COUNT_MSGS 50
#define MAX_COUNT_NICKNAMES 16

Chat* chat;
char nickname[MAX_LENGTH_NICKNAME];

void* receiveNicknames(void* receiveQueueVoid)
{
    mqd_t* receiveQueue = (mqd_t*)receiveQueueVoid;
    char clientName[MAX_LENGTH_NICKNAME];
    unsigned int prio = 0;
    while (1)
    {

        if (mq_receive(*receiveQueue, clientName, MAX_LENGTH_NICKNAME, &prio) == -1)
        {
            perror("Failed receive");
            exit(1);
        }
        if (prio == NICKNAME_PRIO)
        {
            printNickname(chat, clientName, nickname);
        }
        else if (prio == CLEAR_PRIO)
        {
            clearNicknameWin(chat);
        }
    }
}
void* receiveMsgs(void* receiveQueueVoid)
{
    mqd_t* receiveQueue = (mqd_t*)receiveQueueVoid;
    char msg[MAX_LENGTH_MSG];
    unsigned int prio = 0;
    while (1)
    {
        if (mq_receive(*receiveQueue, msg, MAX_LENGTH_MSG, &prio) == -1)
        {
            perror("Failed send");
            exit(1);
        }
        if (prio == NICKNAME_PRIO)
        {
            printMsg(chat, msg);
        }
        else if (prio == CLEAR_PRIO)
        {
            clearMsgWin(chat);
        }
    }
}
void sendMsgInChat(mqd_t* msgSndServerQueue)
{
    char msg[MAX_LENGTH_MSG];
    while (1)
    {
        enterMsg(chat, msg, MAX_LENGTH_MSG);
        if (strcmp(msg, "exit") == 0)
        {
            return;
        }
        if (mq_send(*msgSndServerQueue, msg, strlen(msg), MSG_PRIO) == -1)
        {
            perror("Failed send");
            exit(1);
        }
    }
}

int main()
{

    char nicknames[MAX_COUNT_NICKNAMES][MAX_LENGTH_NICKNAME];
    char msgs[MAX_COUNT_MSGS][MAX_LENGTH_MSG];

    char bufNick[MAX_LENGTH_NICKNAME + 1] = "/";
    char msgQueueName[MAX_LENGTH_NICKNAME + 4] = "/";

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_COUNT_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;

    mqd_t serviceServerQueue = mq_open("/serviceServerQueue", O_WRONLY);
    if (serviceServerQueue == -1)
    {
        perror("Failed open service queue");
        exit(EXIT_FAILURE);
    }
    mqd_t msgSndServerQueue = mq_open("/msgSndServerQueue", O_WRONLY);
    if (msgSndServerQueue == -1)
    {
        perror("Failed open msg queue");
        exit(EXIT_FAILURE);
    }

    printf("Enter nickname\n");
    scanf("%19s", nickname);
    strcat(bufNick, nickname);
    mqd_t serviceClientQueue = mq_open(bufNick, O_RDONLY | O_CREAT, 0600, &attr);
    if (serviceClientQueue == -1)
    {
        perror("Failed create service queue");
        exit(EXIT_FAILURE);
    }

    snprintf(msgQueueName, MAX_LENGTH_NICKNAME + 4, "/msg%s", nickname);
    mqd_t msgClientQueue = mq_open(msgQueueName, O_RDONLY | O_CREAT, 0600, &attr);
    if (msgClientQueue == -1)
    {
        perror("Failed create msg queue");
        exit(EXIT_FAILURE);
    }

    initChat();
    chat = createChat();
    refreshChat(chat);

    pthread_t nicknameThread;
    pthread_t msgThread;

    pthread_create(&nicknameThread, NULL, receiveNicknames, (void*)&serviceClientQueue);
    pthread_create(&msgThread, NULL, receiveMsgs, (void*)&msgClientQueue);


    if (mq_send(serviceServerQueue, nickname, strlen(nickname) + 1, NICKNAME_PRIO) == -1)
    {
        perror("Failed send");
        exit(1);
    }
    
    sendMsgInChat(&msgSndServerQueue);


    pthread_cancel(nicknameThread);
    pthread_cancel(msgThread);
    destroyChat(chat);

    if (mq_close(serviceClientQueue) == -1)
    {
        perror("Failed close");
        exit(1);
    }
    if (mq_close(msgSndServerQueue) == -1)
    {
        perror("Failed close");
        exit(1);
    }
    if (mq_close(msgClientQueue) == -1)
    {
        perror("Failed close");
        exit(1);
    }
    if (mq_unlink(bufNick) == -1)
    {
        perror("Failed unlink");
        exit(1);
    }
    if (mq_unlink(msgQueueName) == -1)
    {
        perror("Failed unlink");
        exit(1);
    }

    if (mq_send(serviceServerQueue, "\0", 2, DIED_PRIO) == -1)
    {
        perror("Failed send");
        exit(1);
    }
    if (mq_close(serviceServerQueue) == -1)
    {
        perror("Failed close");
        exit(1);
    }
    return 0;
}
