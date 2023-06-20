#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int is_parent = 1;  // flaga określająca czy proces jest rodzicem


void signal_handler(int signum)
{
   printf("PID: %d Received signal: %d\n", getpid(), signum);
}
// maskuje sygnał SIGUSR1
void mask_signal()
{
   sigset_t mask_set;
   sigemptyset(&mask_set);
   sigaddset(&mask_set, SIGUSR1);
   if (sigprocmask(SIG_BLOCK, &mask_set, NULL) == -1)
   {
       perror("sigprocmask");
       exit(EXIT_FAILURE);
   }
}

void handle_ignore()
{
   signal(SIGUSR1, SIG_IGN);
   printf("PID: %d Raising signal\n", getpid());
   raise(SIGUSR1);
}

void handle_handler()
{
   signal(SIGUSR1, signal_handler);
   printf("PID: %d Raising signal\n", getpid());
   raise(SIGUSR1);
}

void handle_mask()
{
   if (is_parent)
   {
       mask_signal();
       printf("PID: %d Raising signal\n", getpid());
       raise(SIGUSR1);
   }
}

void handle_pending()
{
   if (is_parent){
       mask_signal();
       printf("PID: %d Raising signal\n", getpid());
       raise(SIGUSR1);
   }

   sigset_t pending_set;

   if (sigpending(&pending_set) == -1){
       perror("sigpending");
       exit(EXIT_FAILURE);
   }

   if (sigismember(&pending_set, SIGUSR1)){
       printf("PID: %d Signal is pending\n", getpid());
   }
   else{
       printf("PID: %d Signal is not pending\n", getpid());
   }
}

void handle_unknown(char* signal_option)
{
   fprintf(stderr, "Unknown argument: %s\n", signal_option);
   exit(EXIT_FAILURE);
}
// funkcja obsługująca argumenty wywołania programu
void handle_signal_option(char* signal_option)
{
   if (strcmp(signal_option, "ignore") == 0)
   {
       handle_ignore();
   }
   else if (strcmp(signal_option, "handler") == 0)
   {
       handle_handler();
   }
   else if (strcmp(signal_option, "mask") == 0)
   {
       handle_mask();
   }
   else if (strcmp(signal_option, "pending") == 0)
   {
       handle_pending();
   }
   else
   {
       handle_unknown(signal_option);
   }
}

int main(int argc, char **argv)
{
   if (argc != 2)
   {
       fprintf(stderr, "Wrong arguments number!\n");
       exit(EXIT_FAILURE);
   }

   char *signal_option = argv[1];

   handle_signal_option(signal_option);

   fflush(NULL);

   is_parent = fork();
   if (!is_parent)
   {
       handle_signal_option(signal_option);
       fflush(NULL);
   }
   else
   {
       wait(NULL);
   }
   exit(EXIT_SUCCESS);
}


