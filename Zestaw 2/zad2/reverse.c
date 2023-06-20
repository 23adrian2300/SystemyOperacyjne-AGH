#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#define BUFFER_SIZE 1024


void block_reverse(const char *source_file, const char *destination_file)
{   // otworz plik do odczytu
    FILE *source = fopen(source_file, "r");
    if (source == NULL)
    {
        printf("Fail while opening source file\n");
        exit(0);
    }
    // otworz plik do zapisu
    FILE *destination = fopen(destination_file, "a");
    if (destination == NULL)
    {
        printf("Fail while opening destination file\n");
        exit(0);
    }
    //dla lepszego pokazania wynikow
    fprintf(destination, "\n-----------------\nWynik dzialania dla block_reverse\n-----------------\n");
    fseek(source, 0, SEEK_END);
    // file_size - rozmiar pliku
    long file_size = ftell(source);
    fseek(source, 0, SEEK_SET);
    // buffer - bufor do odczytu
    char buffer[BUFFER_SIZE];
    // to_read_bytes - liczba bajtów do odczytania
    int to_read_bytes = BUFFER_SIZE;
    // odczytaj bajty z pliku źródłowego i zapisz je w odwróconej kolejności do pliku docelowego
    for (int i = file_size; i > -BUFFER_SIZE; i -= BUFFER_SIZE)
    {   // jeśli i < 0 to odczytujemy tylko tyle bajtów ile zostało do końca pliku
        if (i < 0){
            to_read_bytes = BUFFER_SIZE + i;
            i = 0;
        }
        // ustawienie wskaźnika na początek pliku
        fseek(source, i, SEEK_SET);
        // read_bytes - liczba odczytanych bajtów
        size_t read_bytes = fread(buffer, sizeof(char), to_read_bytes, source);
        // reverse_buffer - bufor do zapisu
        char reverse_buffer[to_read_bytes];
        // odwrócenie kolejności bajtów
        for (int j = 0; j < read_bytes; j++)
        {    
            reverse_buffer[j] = buffer[read_bytes - 1 - j];
        }
        // zapisanie odwróconego bufora do pliku
        fwrite(reverse_buffer, sizeof(char), read_bytes, destination);
    }
    fclose(source);
    fclose(destination);
}


void char_reverse(const char *source_file, const char *destination_file)
{   // otworz plik do odczytu
    FILE *source = fopen(source_file, "r");
    if (source == NULL)
    {
        printf("Fail while opening source file\n");
        exit(0);
    }
    // otworz plik do zapisu
    FILE *destination = fopen(destination_file, "wa");
    if (destination == NULL)
    {
        printf("Fail while opening destination file\n");
        exit(0);
    }
    //dla lepszego pokazania wynikow
    fprintf(destination, "-----------------\nWynik dzialania dla char_reverse\n-----------------\n");
    fseek(source, 0, SEEK_END);
    // file_size - rozmiar pliku
    long file_size = ftell(source);
    fseek(source, 0, SEEK_SET);
    // buffer - bufor do odczytu
    char *buffer = calloc(1, sizeof(char));
    for (int i = file_size -1; i >= 0; i-=1){   
        // odczytaj bajt z pliku źródłowego i zapisz go w odwróconej kolejności do pliku docelowego
        fseek(source, i, SEEK_SET);
        fread(buffer, sizeof(char), 1, source);
        fwrite(buffer, sizeof(char), 1, destination);
    }
    fclose(source);
    fclose(destination);
    free(buffer);
}

void print_time_char(char* source_file, char* destination_file, FILE* report_file) {
   struct timeval start, end;
    // pobierz czas początkowy
    gettimeofday(&start, NULL);

    // wykonaj operacje 
    char_reverse(source_file, destination_file);
    
    // pobierz czas końcowy
    gettimeofday(&end, NULL);
    
    // oblicz czas
    double char_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0; 
    
    // zapisz czas do pliku
    fprintf(report_file, "char_reverse: %f s\n", char_time);
}

void print_time_block(char* source_file, char* destination_file, FILE* report_file) {
    struct timeval start, end;
    // pobierz czas początkowy
    gettimeofday(&start, NULL);

    // wykonaj operacje 
    block_reverse(source_file, destination_file);
    
    // pobierz czas końcowy
    gettimeofday(&end, NULL);
    
    // oblicz czas
    double block_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0; 
    
    // zapisz czas do pliku
    fprintf(report_file, "block_reverse: %f s\n", block_time);
}


int main(int argc, char* argv[]) {
    // sprawdzanie liczby argumentow
	if (argc != 3) {
		printf("Must be exactly 3 arguments\n");
		exit(0);
	}

    // wczytywanie argumentow
	char* source_file = argv[1];
	char* destination_file = argv[2];
	char* report_file = "pomiar_zad_2.txt";

    // otworz plik do zapisu
	FILE* report = fopen(report_file, "wa");
	if (report_file == NULL) {
        printf("Fail while opening report file\n");
        exit(0);
    }
    // wyswietlanie czasu wykonania
    print_time_char(source_file, destination_file, report); //najpierw odpalam to bo czysci plik w ktorym zapisujemy wyniki
    print_time_block(source_file, destination_file, report);

    // zamkniecie pliku
	fclose(report);
	return 0;
}

