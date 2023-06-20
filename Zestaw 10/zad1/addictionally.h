#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

#define MESSAGE_LENGTH 1024
#define NICK_LENGTH 1024
#define MAX_NUMBER_OF_CLIENTS 19
#define PING_TIMER 10


typedef enum
{
    PING,
    NICK,
    LIST,
    TOONE,
    TOALL,
    FULL,
    GET,
    STOP,
} MESSAGETYPE;



typedef struct MESSAGE
{
    MESSAGETYPE type;
    char content[MESSAGE_LENGTH];
    char another[NICK_LENGTH];
} MESSAGE;


typedef enum EVENT
{
    SOCKET,
    CLIENT
} EVENT;


typedef enum
{
    EMPTY = 0,
    INIT,
    READY
} stateClient;


typedef struct client
{
    int fd;
    stateClient state;
    char nick[NICK_LENGTH];
    bool respond;
} client;


typedef union{
    int socket;
    client *client;
} data;

