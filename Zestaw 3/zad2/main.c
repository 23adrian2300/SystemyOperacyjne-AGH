#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>



int main(int argc, char **argv){
    if (argc != 2){
        printf("Niepoprawna liczba argumentow");
        return 1;
    }
    
    printf("Nazwa programu: %s ", argv[0]);

    //brak buforowania danych wejsciowych
    fflush(stdout);

    execl("/bin/ls", "ls", argv[1], NULL);
    return 0;

}