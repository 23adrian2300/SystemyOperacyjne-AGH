#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#define BUF_SIZE 512


double function(double x)
{
    return 4 / (x * x + 1);
}


double integral_function(double a, double b, double width)
{
    double sum = 0;
    double x = a;
    while (x < b)
    {
        sum += function(x) * width;
        x += width;
    }
    return sum;
}


double compute_integral_using_pipes(int n, double width) {
    double sum = 0;
    double x = 1.0 / n;
    int pipes[n][2];


    for (int i = 0; i < n; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Error creating pipe");
            exit(1);
        }


        pid_t pid = fork();

        if (pid == 0) {
            close(pipes[i][0]);
            double result = integral_function(x * i, x * (i + 1), width);
            char buf[BUF_SIZE];
            int len = snprintf(buf, BUF_SIZE, "%f", result);
            write(pipes[i][1], buf, len + 1);
            close(pipes[i][1]);
            exit(0);
        } else {
            close(pipes[i][1]);
        }
    }


    for (int i = 0; i < n; i++) {
        char buf[BUF_SIZE];
        int len = read(pipes[i][0], buf, BUF_SIZE);
        if (len == -1) {
            perror("Error reading from pipe");
            exit(1);
        }
        sum += atof(buf);
        close(pipes[i][0]);
    }


    while (wait(NULL) > 0);


    return sum;
}



int main(int argc, char **argv)
{
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
    if (width * n > 1)
    {
        printf("Width * n > 1\n");
        exit(1);
    }
    clock_t start, end;
    double cpu_time_used;
    struct timeval start1, end1;
    double total_time = 0;
    gettimeofday(&start1, NULL);
    start = clock();
    double sum = compute_integral_using_pipes(n, width);
    end = clock();
    gettimeofday(&end1, NULL);
    total_time = (end1.tv_sec - start1.tv_sec)+((end1.tv_usec - start1.tv_usec)/1000000.0);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    FILE *f = fopen("report.txt", "a");
    fprintf(f,"RESULT: %lf\nN: %d\nWidth: %.10f\nCPU time: %lf s\nReal time: %f s\n\n", sum, n, width, cpu_time_used, total_time);
    fclose(f);
    printf("RESULT: %lf\nN: %d\nWidth: %.10f\nCPU time: %lf s\nReal time: %f s\n\n", sum, n, width, cpu_time_used, total_time);



    return 0;
}

