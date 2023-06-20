#ifndef TYPICAL_H
#define TYPICAL_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#define MAX_MESSAGE_LEN 256
#define MAX_NO_USERS 10
#define PROJECT_ID 1
#define SERVER_QUEUE "/server_queue"


typedef struct {
    char mess[MAX_MESSAGE_LEN];
    pid_t sender_pid;
    int receiver_id;
    struct tm tm;
} message;

typedef enum{
    STOP = 1,
    LIST = 2,
    TO_ALL = 3,
    TO_ONE = 4,
    INIT = 5
} m_type;


const int size = sizeof(message);

mqd_t create_queue(const char* homepath) {
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = size;
    mqd_t msqid = mq_open(homepath, O_RDONLY | O_CREAT, 0666, &attr);
    if (msqid== -1) {
        perror("Error creating queue\n");
        exit(EXIT_FAILURE);
    }

    return msqid;
}


mqd_t open_queue(const char* homepath) {
    mqd_t msqid = mq_open(homepath, O_WRONLY);
    if (msqid== -1) {
        perror("Error: could not open queue\n");
        exit(EXIT_FAILURE);
    }
    return msqid;
}

#endif //TYPICAL_H