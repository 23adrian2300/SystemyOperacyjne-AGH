#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

char buf[2048];


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



int main(int argc, char **argv)
{
    double width = atof(argv[1]);
    double a = atof(argv[2]);
    double b = atof(argv[3]);


    double result = integral_function(a, b, width);
    size_t size;
    size = sprintf(buf, "%lf ", result);
    int fifo = open("/tmp/workerr", O_WRONLY);
    write(fifo, buf, size);
    close(fifo);
    return 0;
}
