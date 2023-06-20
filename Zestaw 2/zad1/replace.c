#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#define BUFFOR_SIZE 1024


void sys_replace(const char *char_to_search, const char *char_to_replace, const char *source_filename, const char *destination_filename)
{
    // otworz plik do odczytu
    int source= open(source_filename, O_RDONLY);
    if (source== -1)
    {
        printf("Fail with source file\n");
        exit(0);
    }


    // otworz plik do zapisu
    int destination = open(destination_filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (destination == -1)
    {
        printf("Fail with destination file\n");
        exit(0);
    }

    char buffor[BUFFOR_SIZE];
    size_t read_bytes, write_bytes;

    // szukanie i zamiana znaku
    while ((read_bytes= read(source, buffor, BUFFOR_SIZE)) > 0)
    {
        for (int i = 0; i < read_bytes; i++)
        {
            if (buffor[i] == char_to_search)
            {
                buffor[i] = char_to_replace;
            }
        }
        write_bytes = write(destination, buffor, read_bytes);
        if (write_bytes == -1)
        {
            printf("Fail writing to destination file.\n");
            exit(0);
        }
    }


    // check for read error
    if (read_bytes == -1)
    {
        printf("Fail reading from source file.\n");
        exit(0);
    }


    // close file descriptors
    close(source);
    close(destination);
}


void lib_replace(const char *char_to_search, const char *char_to_replace, const char *source_filename, const char *destination_filename)
{
    // otworz plik do odczytu
    FILE *source_file = fopen(source_filename, "rb");
    if (source_file == NULL)
    {
        printf("Fail with source file\n");
        exit(0);
    }


    // otworz plik do zapisu
    FILE *destination_file = fopen(destination_filename, "ab");
    if (destination_file == NULL)
    {
        printf("Fail with destination file\n");
        exit(0);
    }

    fprintf(destination_file, "\n-----------------\nNa gorze wynik dzialania sys_replace\nPonizej wynik dzialania lib_replace\n-----------------\n");
    char buffor[BUFFOR_SIZE];
    size_t read_bytes, write_bytes;
    // szukanie i zamiana znaku
    while ((read_bytes = fread(buffor, 1, BUFSIZ, source_file)) > 0)
    {
        for (int i = 0; i < read_bytes; i++)
        {
            if (buffor[i] == char_to_search)
            {
                buffor[i] = char_to_replace;
            }
        }
        write_bytes = fwrite(buffor, 1, read_bytes, destination_file);
        if (write_bytes != read_bytes)
        {
            printf("Fail writing to destination file.\n");
            exit(0);
        }
    }


    // sprawdz czy wystapil blad podczas odczytu
    if (ferror(source_file))
    {
        printf("Fail reading from source file.\n");
        exit(0);
    }


    // zamknij pliki
    fclose(source_file);
    fclose(destination_file);
}

void print_time(double sys_time, double lib_time, FILE *results_file){
    fprintf(results_file, "Sys_replace time: %.6f s\n", sys_time); 
    fprintf(results_file, "Lib_replace time: %.6f s\n", lib_time); 
}


int main(int argc, char *argv[]) { 
    // sprawdz czy podano poprawna liczbe argumentow
    if (argc != 5) { 
        printf("Program requires exactly 4 command-line arguments.\n"); 
        exit(0);
        } 
        // pobierz argumenty
        char char_to_search = argv[1][0]; 
        char char_to_replace = argv[2][0]; 
        char *source_filename = argv[3]; 
        char *destination_filename = argv[4]; 

        // zmienne do mierzenia czasu
        struct timeval start, end; 
        
        // mierz czas dla funkcji sys_replace
        gettimeofday(&start, NULL); 
        sys_replace(char_to_search, char_to_replace, source_filename, destination_filename); 
        gettimeofday(&end, NULL); 
        double sys_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0; 

        // mierzenie czasu dla funkcji lib_replace
        gettimeofday(&start, NULL); 
        lib_replace(char_to_search, char_to_replace, source_filename, destination_filename); 
        gettimeofday(&end, NULL); 
        double lib_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0; 

        // zapis wynikow do pliku
        FILE *results_file = fopen("pomiar_zad_1.txt", "w"); 
        
        // sprawdz czy plik zostal otwarty poprawnie
        if (results_file == NULL) { 
            printf("Fail to open results file for writing.\n"); 
            exit(0); 
            }
        print_time(sys_time, lib_time, results_file);
        fclose(results_file); 

        return 0; 
        } 
        