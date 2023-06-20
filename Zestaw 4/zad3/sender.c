#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>


int received;


void handler(int signum)
{
    printf("Sender>> Received signal\n");
    received = 1;
}


int main(int argc, char **argv)
{
    // sprawdzamy czy podano odpowiednia ilosc argumentow
    if (argc < 3)
    {
        printf("Sender>> Not enough arguments\n");
        exit(1);
    }
    // pobieramy pid catchera
    int catcherpid = atoi(argv[1]);


    // wysylamy sygnaly dla kazdego moda
    for (int i = 2; i < argc; i++)
    {
        int mode = atoi(argv[i]);


        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = handler;
        sigaction(SIGUSR1, &action, NULL);


        sigval_t value = {mode};
        sigqueue(catcherpid, SIGUSR1, value);
        printf("Sender>> Sent mode %d\n", mode);


        // czekamy na potwierdzenie
        printf("Sender>> Waiting for confirmation\n");
        while (!received);
    }
}
