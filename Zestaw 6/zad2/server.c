#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/stat.h>

#include "typical.h"

mqd_t msqid;
int pid_of_users[MAX_NO_USERS ];
mqd_t users_queue_pid[MAX_NO_USERS ];
char log[MAX_MESSAGE_LEN];
FILE* log_fd;


void handle_init(message* message_buffor) {
    int i = 0;
    for (; i < MAX_NO_USERS ; i++) {
        if (pid_of_users[i] == 0) {
            pid_of_users[i] = message_buffor->sender_pid;
            users_queue_pid[i] = open_queue(message_buffor->mess);
            break;
        }
    }
    message msg;
    msg.sender_pid = getpid();
    sprintf(msg.mess, "%d", i);
    if (mq_send(users_queue_pid[i], (char *) &msg, size, INIT) == -1) {
        perror("[SERVER] could not send INIT message\n");
        exit(1);
    }
}


void handle_2one(message* message_buffor) {
    time_t tm = time(NULL);
    message_buffor->tm = *localtime(&tm);

    if (mq_send(users_queue_pid[message_buffor->receiver_id], (char *) message_buffor, size, TO_ONE) == -1) {
        perror("[SERVER] could not send TO_ONE message\n");
        exit(1);
    }
    kill(pid_of_users[message_buffor->receiver_id], SIGUSR1);
}


void handle_2all(message* message_buffor) {
    time_t tm = time(NULL);
    message_buffor->tm = *localtime(&tm);

    for (int i = 0; i < MAX_NO_USERS ; i++) {
        if (pid_of_users[i] != 0 && i != message_buffor->sender_pid) {
            if (mq_send(users_queue_pid[i], (char *) message_buffor, size, TO_ALL) == -1) {
                perror("[SERVER] could not send TO_ALL message\n");
                exit(1);
            }
            kill(pid_of_users[i], SIGUSR1);
        }
    }
}


void handle_list(message* message_buffor) {
    printf("[Users at the moment]\n");
    for (int i = 0; i < MAX_NO_USERS ; i++) {
        if (pid_of_users[i] != 0) {
            printf("- %d", i);
            if (i == message_buffor->sender_pid) {
                printf("(you)");
            }
            printf("\n");
        }
    }
    printf("=====END=====\n");
}


void handle_disconect(message* message_buffor) {
    int i = message_buffor->sender_pid;
    pid_of_users[i] = 0;
    users_queue_pid[i] = 0;
}


void process_message(message* message_buffor, uint type) {
   if (type == STOP) {
       handle_disconect(message_buffor);
   }
   else if (type == LIST) {
       handle_list(message_buffor);
   }
   else if (type == TO_ALL) {
       handle_2all(message_buffor);
   }
   else if (type == TO_ONE) {
       handle_2one(message_buffor);
   }
   else if (type == INIT) {
       handle_init(message_buffor);
   }
   write_to_log(message_buffor, type);
}

void close_q() {
    fprintf(log_fd, "closing\n");
    fclose(log_fd);
    for (int i = 0; i < MAX_NO_USERS ; i++) {
        if (pid_of_users[i] != 0) {
            mq_close(users_queue_pid[i]);
            kill(pid_of_users[i], SIGINT);
        }
    }
    mq_unlink(SERVER_QUEUE);
    exit(EXIT_SUCCESS);
}
void write_to_log(message* message_buffor, uint type) {
   char buff[512];
   strcpy(buff, "");
   time_t tim = time(NULL);
   struct tm tm = *localtime(&tim);
   if (type == STOP) {
       fprintf(log_fd, "STOP %d\n", message_buffor->sender_pid);
   }
   else if (type == LIST) {
       fprintf(log_fd, "LIST requested by %d\n", message_buffor->sender_pid);
   }
   else if (type == TO_ALL) {
       fprintf(log_fd, "TO_ALL from %d\n", message_buffor->sender_pid);
       fprintf(log_fd, "%s\n", message_buffor->mess);
   }
   else if (type == TO_ONE) {
       fprintf(log_fd, "TO_ONE from %d to %d\n", message_buffor->sender_pid, message_buffor->receiver_id);
       fprintf(log_fd, "%s\n", message_buffor->mess);
   }
   else if (type == INIT) {
       fprintf(log_fd, "INIT %d\n", message_buffor->sender_pid);
   }


   fprintf(log_fd, "\n%02d-%02d %02d:%02d\n\n",
            tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
}


int main() {
    if (signal(SIGINT, close_q) == SIG_ERR) {
        perror("[SERVER] problem with SIGINT\n");
        exit(1);
    }
    strcpy(log, "logs.txt");
    if ((log_fd = fopen(log, "w")) == NULL) {
        perror("[SERVER] cannot open logs.txt\n");
        exit(1);
    }
    msqid = create_queue(SERVER_QUEUE);
    message message_buffor;
    uint type;
    while (1) {
        if (mq_receive(msqid, (char *) &message_buffor, size, &type) == -1) {
            perror("[SERVER] could not receive message\n");
            exit(1);
        }

        process_message(&message_buffor, type);
    }
}