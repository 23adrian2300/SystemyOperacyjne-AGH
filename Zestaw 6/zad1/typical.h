#ifndef TYPICAL_H
#define TYPICAL_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <time.h>


#define MAX_MESSAGE_SIZE 256
#define MAX_NO_USERS 10
#define PROJECT_ID 1
#define HOMEPATH getenv("HOME") 

typedef struct
{
    long type;
    char mess[MAX_MESSAGE_SIZE];
    pid_t pid_sender;
    int receiver_id;
    struct tm time_st;
} message;


typedef enum{
    STOP = 1,
    LIST = 2,
    TO_ALL = 3,
    TO_ONE = 4,
    INIT = 5
} m_type;


int create_queue(const char *path, char proj_id)
{   // tworzenie klucza
    key_t key = ftok(path, proj_id);
    if (key == -1)
    {
        perror("Error with ftok\n");
        exit(EXIT_FAILURE);
    }
    // tworzenie kolejki
    int msqid = msgget(key, IPC_CREAT  | 0666);
    if (msqid == -1)
    {
        perror("Error creating queue\n");
        exit(EXIT_FAILURE);
    }
    return msqid;
}


#endif // TYPICAL_H



