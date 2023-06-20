#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>


int request_count = 0;
int requested_mode = 0;
int working;


typedef enum
{
   DISPLAY_NUMBERS = 1,
   ONE_TIME = 2,
   DISPLAY_REQUEST_COUNT = 3,
   LOOP_TIME = 4,
   KILL = 5,
} modes_types;




void handler(int signum, siginfo_t *info, void *context)
{  // siginfo_t *info - struktura zawierająca dodatkowe informacje o sygnale
   int pid = info->si_pid;
   int requested_mode_t = info->si_status;

    
   if (requested_mode_t < 1 || requested_mode_t > 5)
   {
       fprintf(stderr, "Catcher>> Invalid request (%d).\n", request_count);
   }
   else
   {
       request_count += 1;
       requested_mode = (modes_types)requested_mode_t;
       working = 1;
   }


   kill(pid, SIGUSR1);
}



int main(int argc, char **argv)
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);


    printf("CatcherR>> PID: %d\n", getpid());


    while (1)
    {
        if (working == 0)
        {
            printf("Catcher>> Waiting for signal\n");
            pause();
        }
        switch (requested_mode)
        {
        case DISPLAY_NUMBERS:
            for (int i = 1; i < 101; i++)
            {
                printf("Catcher>> %d\n", i);
            }
            working = 0;
            break;
        case ONE_TIME:
            static time_t one_time_raw_time;
            static struct tm *one_time_time_info;
            time(&one_time_raw_time);
            one_time_time_info = localtime(&one_time_raw_time);
            printf("Catcher>> Current time: %s", asctime(one_time_time_info));
            sleep(1);
            working = 0;
            break;
        case DISPLAY_REQUEST_COUNT:
            printf("Catcher>> Requests count: %d\n", request_count);
            working = 0;
            break;
        case LOOP_TIME:
            static time_t loop_time_raw_time;
            static struct tm *loop_time_time_info;
            time(&loop_time_raw_time);
            loop_time_time_info = localtime(&loop_time_raw_time);
            printf("Catcher>> Current time: %s", asctime(loop_time_time_info));
            sleep(1);
            working = 1;
            break;
        case KILL:
            printf("Catcher>> Exit\n");
            exit(EXIT_SUCCESS);
            break;
        default:
            printf("Catcher>> Wrong mode\n");
            pause();
            break;
        }
        fflush(NULL);
    }


    exit(EXIT_SUCCESS);
}
