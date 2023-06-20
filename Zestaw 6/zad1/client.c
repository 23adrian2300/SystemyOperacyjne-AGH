#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "typical.h"


int msqid;
int queue;
int ID;
const int size = sizeof(message) - sizeof(long);

void handle_message()
{
    message msg;
    if (msgrcv(queue, &msg, size, -INIT - 1, 0) == -1)
    {
        perror("[Client] could not receive a message from server\n");
        exit(EXIT_FAILURE);
    }
    struct tm tm = msg.time_st;
    printf("Message (FROM ID: %d): \n%s\n%02d-%02d %02d:%02d\n", msg.pid_sender, msg.mess,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
}

void handle_list()
{
   message msg;
   msg.type = LIST;
   msg.pid_sender = msqid;

    fflush(stdout);
   // Send a message to the server requesting a list of users
   if (msgsnd(msqid, &msg, size, 0) == -1)
   {
       perror("[Client] could not send LIST message to server\n");
       exit(EXIT_FAILURE);
   }


   // Wait for the response message from the server
   if (msgrcv(msqid, &msg, size, LIST, 0) == -1)
   {
       perror("[Client] could not receive LIST message from server\n");
       exit(EXIT_FAILURE);
   }


   // Display the list of users received from the server
   printf("[List of users]\n%s", msg.mess);
}



void handle_2all(char *content)
{
    message msg;
    msg.type = TO_ALL;
    msg.pid_sender = ID;
    strcpy(msg.mess, content);
    if (msgsnd(msqid, &msg, size, 0) == -1)
    {
        perror("[Client] could not send TO_ALL message to server\n");
        exit(EXIT_FAILURE);
    }
}
void handle_2one(int receiver_id, char *content)
{
    message msg;
    msg.type = TO_ONE;
    msg.pid_sender = ID;
    msg.receiver_id = receiver_id;
    strcpy(msg.mess, content);
    if (msgsnd(msqid, &msg, size, 0) == -1)
    {
        perror("[Client] could not send TO_ONE message to server\n");
        exit(EXIT_FAILURE);
    }
}


void handle_ending()
{
    msgctl(queue, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}


void handle_stop()
{
    msgctl(queue, IPC_RMID, NULL);
    message msg;
    msg.type = STOP;
    msg.pid_sender = ID;
    if (msgsnd(msqid, &msg, size, 0) == -1)
    {
        perror("[Client] could not send STOP message to server\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}


void handle_init()
{
    message to_server;
    to_server.type = INIT;
    to_server.pid_sender = getpid();
    sprintf(to_server.mess, "%d", queue);
    if (msgsnd(msqid, &to_server, size, 0) == -1)
    {
        perror("[Client] could not send INIT message to server\n");
        exit(EXIT_FAILURE);
    }
    message from_server;
    if (msgrcv(queue, &from_server, size, INIT, 0) == -1)
    {
        perror("[Client] could not receive INIT message from server\n");
        exit(EXIT_FAILURE);
    }
    ID = atoi(from_server.mess);
    fflush(stdout);
}


int main()
{
    if (atexit(handle_ending) != 0)
    {
        printf("[Client] problem with atexit\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, handle_stop) == SIG_ERR)
    {
        printf("[Client] problem with SIGINT handler\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGUSR1, handle_message) == SIG_ERR)
    {
        printf("[Client] problem with SIGUSR1 handler\n");
        exit(EXIT_FAILURE);
    }
    if (HOMEPATH == NULL)
    {
        perror("[Client] could not get home_path\n");
        exit(EXIT_FAILURE);
    }
    key_t key = ftok(HOMEPATH, PROJECT_ID);
    if (key == -1)
    {
        perror("[Client] could not get the queue key\n");
        exit(EXIT_FAILURE);
    }
    msqid = msgget(key, 0);
    if (msqid == -1)
    {
        perror("[Client] could not get the server queue\n");
        exit(EXIT_FAILURE);
    }
    queue = create_queue(HOMEPATH, (getpid() % ('z' - 'a' + 1)) + 'a');
    handle_init();
    char input[MAX_MESSAGE_SIZE];
    char *token;
    char content[MAX_MESSAGE_SIZE];
    while (1)
    {
        fgets(input, MAX_MESSAGE_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;
        token = strtok(input, " ");
        if (strcmp(token, "STOP") == 0)
        {
            handle_stop();
        }
        else if (strcmp(token, "LIST") == 0)
        {
            handle_list();
        }
        else if (strcmp(token, "2ALL") == 0)
        {
            token = strtok(NULL, "\n");
            sprintf(content, "%s", token);
            handle_2all(content);
        }
        else if (strcmp(token, "2ONE") == 0)
        {
            int receiver_id = -1;
            token = strtok(NULL, " ");
            receiver_id = atoi(token);
            token = strtok(NULL, "\n");
            sprintf(content, "%s", token);
            handle_2one(receiver_id, content);
        }
    }
}



