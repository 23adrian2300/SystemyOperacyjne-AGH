#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/time.h>

char rbuf[10240]="";
char a[2048]= "";
char b[2048]="";

int main(int argc, char **argv){
    
      if (argc != 3)
    {
        printf("Wrong number of arguments");
        exit(1);
    }
    double width = atof(argv[1]);
    if (width <= 0)
    {
        printf("Width <= 0");
        exit(1);
    }

    int n = atoi(argv[2]);

    clock_t start, end;
    double cpu_time_used;
    struct timeval start1, end1;
    double total_time = 0;
    gettimeofday(&start1, NULL);
    if (width * n > 1)
    {
        printf("Width * n > 1\n");
        exit(1);
    }

    double sum = 0.0;
    double x = 1.0 / n;
    mkfifo("/tmp/workerr", 0666);
    int i;
    for (i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {   
            sprintf(a, "%lf", x * i);
            sprintf(b, "%lf", x * (i + 1));
            execl("./worker", "worker",argv[1], a, b, NULL);
        }
    }
    
    int fifo = open("/tmp/workerr", O_RDONLY);
    int readed = 0;


    while (readed < n)
    {
        size_t size = read(fifo, rbuf, 10240); //tak duze aby nie bylo problemow z przekroczeniem bufora
        rbuf[size] = 0;
        char delim[] = " ";
        char *token;
        token = strtok(rbuf, delim);
        for (; token; token = strtok(NULL, delim))
        {   
            sum += atof(token);
            readed++;
        }
    }

    close(fifo);
    remove("/tmp/workerr");


    end = clock();
    gettimeofday(&end1, NULL);
    total_time = (end1.tv_sec - start1.tv_sec)+((end1.tv_usec - start1.tv_usec)/1000000.0);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    FILE *report = fopen("report.txt", "a");

    fprintf(report,"RESULT: %lf\nN: %d\nWidth: %.10f\nCPU time: %lf s\nReal time: %f s\n\n", sum, n, width, cpu_time_used, total_time);
    fclose(report);
    printf("RESULT: %lf\nN: %d\nWidth: %.10f\nCPU time: %lf s\nReal time: %fs \n\n", sum, n, width, cpu_time_used, total_time);
    return 0;
}
