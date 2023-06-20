#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <mqueue.h>

#include "typical.h"
mqd_t msqid;
mqd_t queue;
int ID;
char queue_path[250];



void handle_2one(int receiver_id, char* content) {
    message msg;
    msg.sender_pid = ID;
    msg.receiver_id = receiver_id;
    strcpy(msg.mess, content);

    if (mq_send(msqid, (char *) &msg, size, TO_ONE) == -1) {
        perror("[CLIENT] could not send 2ONE message to server\n");
        exit(1);
    }
}


void handle_2all(char* content) {
    message msg;
    msg.sender_pid = ID;
    strcpy(msg.mess, content);
    if (mq_send(msqid, (char *) &msg, size, TO_ALL) == -1) {
        perror("[CLIENT] could not send 2ALL message to server\n");
        exit(1);
    }
}


void handle_list() {
    message msg;
    msg.sender_pid = ID;

    if (mq_send(msqid, (char *) &msg, size, LIST) == -1) {
        perror("[CLIENT] could not send LIST message to server\n");
        exit(1);
    }
}


void handle_ending() {
    mq_close(msqid);
    mq_unlink(queue_path);
    exit(0);
}


void handle_stop() {
    message msg;
    msg.sender_pid = ID;
    msg.mess[0] = '\0';
    if (mq_send(msqid, (char *) &msg, size, STOP) == -1) {
        perror("[CLIENT] could not send STOP message to server\n");
        exit(1);
    }
    mq_close(msqid);
    mq_unlink(queue_path);

    exit(0);
}


void send_init(const char* path) {
    message tserver;
    tserver.sender_pid = getpid();
    sprintf(tserver.mess, "%s", path);

    if (mq_send(msqid, (char *) &tserver, size, INIT) == -1) {
        perror("[CLIENT] could not send INIT message to server\n");
        exit(1);
    }
    message fserver;
    if (mq_receive(queue, (char *) &fserver, size, NULL) == -1) {
        perror("[CLIENT] could not receive INIT message from server\n");
        exit(1);
    }
    ID = atoi(fserver.mess);
    fflush(stdout);
}
void handle_message() {
    message msg;
    uint type;
    if (mq_receive(queue, (char *) &msg, size, &type) == -1) {
        perror("[CLIENT] could not receive a message from server\n");
        exit(1);
    }
    struct tm tm = msg.tm;
    printf("Message (FROM ID:%d): \n%s\n\n%02d-%02d %02d:%02d\n", msg.sender_pid, msg.mess,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);

}


int main() {
    if (atexit(handle_ending) != 0) {
        printf("[CLIENT] problem with atexit\n");
        exit(1);
    }
      if (signal(SIGUSR1, handle_message) == SIG_ERR) {
        printf("[CLIENT] problem with SIGUSR1 handler\n");
        exit(1);
    }

    if (signal(SIGINT, handle_stop) == SIG_ERR) {
        printf("[CLIENT] problem with SIGINT handler\n");
        exit(1);
    }
    sprintf((char *) queue_path, "/%d", getpid());
    msqid = open_queue(SERVER_QUEUE);
    queue = create_queue(queue_path);
    send_init(queue_path);
    char sdtin[MAX_MESSAGE_LEN];
    char* token;
    char content[MAX_MESSAGE_LEN];
    while (1) {
        fgets(sdtin, MAX_MESSAGE_LEN, stdin);
        sdtin[strcspn(sdtin, "\n")] = 0;
        token = strtok(sdtin, " ");
        if (strcmp(token, "STOP") == 0) {
            handle_stop();
        } else if (strcmp(token, "LIST") == 0) {
            handle_list();
        } else if (strcmp(token, "2ALL") == 0) {
            token = strtok(NULL, "\n");
            sprintf(content, "%s", token);
            handle_2all(content);
        } else if (strcmp(token, "2ONE") == 0) {
            int receiver_id = -1;
            token = strtok(NULL, " ");
            receiver_id = atoi(token);
            token = strtok(NULL, "\n");
            sprintf(content, "%s", token);
            handle_2one(receiver_id, content);
        }
    }
}