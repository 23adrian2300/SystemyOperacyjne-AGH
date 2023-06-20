#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc!= 2)
        {
            printf("Niepoprawna liczba argumentow");
            return 1;
        }
    int n = atoi(argv[1]);
    if (n < 0)
    {
        printf("Liczba musi byc dodatnia");
        return 1;
    }
    int dec = n;
    while (dec > 0){
            pid_t pid = fork();
            if (pid== 0)
            {
                printf("Proces %d, rodzic %d\n", getpid(), getppid());
                exit(0);
            }
            else if (pid ==-1)
            {
                printf("Blad fork()");
                exit(1);
            }
            dec--;
        }
    for (int i = 0; i < n; i++)
        wait(NULL);

    printf("Liczba procesow: %d\n", n);
    return 0;
    }
