#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "typical.h"
#include <fcntl.h>


int msqid;
int users_pid[MAX_NO_USERS];
int users_queue_pid[MAX_NO_USERS];
char content[MAX_MESSAGE_SIZE];
const int size = sizeof(message) - sizeof(long);
FILE *log_fd;


void handle_init(message *message_buffer)
{
    int i = 0;
    for (; i < MAX_NO_USERS; i++)
    {
        if (users_pid[i] == 0)
        {
            users_pid[i] = message_buffer->pid_sender;
            users_queue_pid[i] = atoi(message_buffer->mess);
            break;
        }
    }
    message msg;
    msg.pid_sender = getpid();
    msg.type = INIT;
    sprintf(msg.mess, "%d", i);
    if (msgsnd(users_queue_pid[i], &msg, size, 0) == -1)
    {
        perror("[SERVER] could not send INIT message\n");
        exit(EXIT_FAILURE);
    }
}


void handle_2one(message *message_buffer)
{
    message msg;
    msg.type = TO_ONE;
    msg.pid_sender = message_buffer->pid_sender;
    msg.receiver_id = message_buffer->receiver_id;
    time_t tm = time(NULL);
    msg.time_st = *localtime(&tm);
    sprintf(msg.mess, "%s", message_buffer->mess);
    if (msgsnd(users_queue_pid[message_buffer->receiver_id], &msg, size, 0) == -1)
    {
        perror("[SERVER] could not send TO_ONE message\n");
        exit(EXIT_FAILURE);
    }
    kill(users_pid[message_buffer->receiver_id], SIGUSR1);
}


void handle_2all(message *message_buffer)
{
    time_t tm = time(NULL);
    message_buffer->time_st = *localtime(&tm);
    for (int i = 0; i < MAX_NO_USERS; i++)
    {
        if (users_pid[i] != 0 && i != message_buffer->pid_sender)
        {
            if (msgsnd(users_queue_pid[i], message_buffer, size, 0) == -1)
            {
                perror("[SERVER] could not send TO_ALL message\n");
                exit(EXIT_FAILURE);
            }
            kill(users_pid[i], SIGUSR1);
        }
    }
}


void handle_list(message *message_buffer)
{   
    int k = 0;
    // Construct the list of users
    char user_list[MAX_MESSAGE_SIZE] = "";
    for (int i = 0; i < MAX_NO_USERS; i++)
    
    {
        if (users_pid[i] != 0)
        {
            char user_pid_str[10];
            sprintf(user_pid_str, "%d", i);
            strcat(user_list, "- ");
            strcat(user_list, user_pid_str);
            strcat(user_list, "\n");
        }
    }
    strcpy(message_buffer->mess, user_list);
    if (msgsnd(message_buffer->pid_sender, message_buffer, size, 0) == -1)
    {
        perror("[Server] could not send LIST message to client\n");
        exit(EXIT_FAILURE);
    }
}



void process_message(message *message_buffer)
{
    if (message_buffer->type == STOP) {
        int i = message_buffer->pid_sender;
        users_pid[i] = 0;
        users_queue_pid[i] = 0;
    } else if (message_buffer->type == LIST) {
        handle_list(message_buffer);
    } else if (message_buffer->type == TO_ALL) {
        handle_2all(message_buffer);
    } else if (message_buffer->type == TO_ONE) {
        handle_2one(message_buffer);
    } else if (message_buffer->type == INIT) {
        handle_init(message_buffer);
    }
    message_log(message_buffer);
}

void close_queue()
{
    fprintf(log_fd, "Closing\n");
    fclose(log_fd);
    for (int i = 0; i < MAX_NO_USERS; i++)
    {
        if (users_pid[i] != 0)
        {
            kill(users_pid[i], SIGINT);
        }
    }
    msgctl(msqid, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}
void message_log(message *message_buffer)
{
    char buff[512];
    strcpy(buff, "");
    time_t tim = time(NULL);
    struct tm tm = *localtime(&tim);
    if (message_buffer->type == STOP) {
        fprintf(log_fd, "STOP %d\n", message_buffer->pid_sender);
    } 
    else if (message_buffer->type == LIST) {
        fprintf(log_fd, "LIST requested by %d\n", message_buffer->pid_sender);
    } 
    else if (message_buffer->type == TO_ALL) {
        fprintf(log_fd, "TO_ALL from %d\n", message_buffer->pid_sender);
        fprintf(log_fd, "%s\n", message_buffer->mess);
    } 
    else if (message_buffer->type == TO_ONE) {
        fprintf(log_fd, "TO_ONE from %d to %d\n", message_buffer->pid_sender, message_buffer->receiver_id);
        fprintf(log_fd, "%s\n", message_buffer->mess);
    } 
    else if (message_buffer->type == INIT) {
        fprintf(log_fd, "INIT %d\n", message_buffer->pid_sender);
    }
    fprintf(log_fd, "\n%02d-%02d %02d:%02d\n\n",
     tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
}


int main()
{
    if (signal(SIGINT, close_queue) == SIG_ERR)
    {
        perror("[SERVER] problem with SIGINT\n");
        exit(EXIT_FAILURE);
    }
    strcpy(content, "logs.txt");
    if ((log_fd = fopen(content, "w")) == NULL)
    {
        perror("[SERVER] cannot open logs.txt\n");
        exit(EXIT_FAILURE);
    }
    if (HOMEPATH == NULL)
    {
        perror("[SERVER] could not get home_path\n");
        exit(EXIT_FAILURE);
    }
    msqid = create_queue(HOMEPATH, PROJECT_ID);
    message message_buffer;
    while (1)
    {
        if (msgrcv(msqid, &message_buffer, size, -(INIT + 1), 0) == -1)
        {
            perror("[SERVER] could not receive message\n");
            exit(EXIT_FAILURE);
        }


        process_message(&message_buffer);
    }
}



