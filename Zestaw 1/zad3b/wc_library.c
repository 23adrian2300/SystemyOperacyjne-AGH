#include "wc_library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define MAX_COMMAND_LENGTH 512


// inicjalizacja pamieci
wc_memory* wc_memory_init(int max_size) {
   wc_memory* memory = malloc(sizeof(*memory));
   if (memory == NULL) {
       exit(0);
   }
   *memory = (wc_memory) {
       .memory_blocks = calloc(max_size, sizeof(*memory->memory_blocks)),
       .max_size = max_size,
       .current_size = 0
   };
   if (memory->memory_blocks == NULL) {
       free(memory);
       exit(0);
   }
   return memory;
}

// funkcja zliczajaca slowa, znaki i linie w pliku
void counter(wc_memory *memory, char *file_to_count) {
    struct stat st;
    if (stat(file_to_count, &st) != 0) {
        perror("stat() failed");
        exit(0);
    }
    size_t file_size = st.st_size;
    char cmd[MAX_COMMAND_LENGTH];
    snprintf(cmd, MAX_COMMAND_LENGTH - 1, "wc %s > /tmp/tmp_file", file_to_count);
    system(cmd);
    FILE *fp = fopen("/tmp/tmp_file", "r");
    if (fp == NULL) {
        perror("fopen() failed");
        exit(0);
    }
    char *output = calloc(file_size + 1, sizeof(char));
    if (fgets(output, file_size + 1, fp) == NULL) {
        perror("fgets() failed");
        free(output);
        fclose(fp);
        exit(0);
    }
    fclose(fp);
    if (memory->current_size >= memory->max_size) {
        free(output);
        exit(0);
    }
    memory->memory_blocks[memory->current_size] = output;
    memory->current_size++;
    remove("/tmp/count_file");
}

//funkcja zwracajaca zawartosc bloku o podanym indeksie
char *block_content_from_index(wc_memory* memory, int index){
    //sprawdzanie czy pamiec jest pusta
    if (memory->current_size == 0){
        printf("Memory is empty!\n");
        exit(0);
    }
    //sprawdzanie czy index jest poprawny
    if (index < 0 || index >= memory->max_size){
        printf("Index out of range!\n");
        exit(0);
    }
    return memory->memory_blocks[index];
}

//usuwanie bloku z pamieci (zastanowic sie czy nie lepiej przesuwac bloki)
void remove_block_from_index(wc_memory* memory, int index){
    //sprawdzanie czy pamiec jest pusta
    if (memory->current_size == 0){
        printf("Memory is empty!\n");
        exit(0);
    }
    //sprawdzanie czy index jest poprawny
    if (index < 0 || index >= memory->max_size){
        printf("Index out of range!\n");
        exit(0);
    }
    free(memory->memory_blocks[index]);
    memory->memory_blocks[index] = NULL;

}

//funkcja zwalniajaca pamiec
void free_memory(wc_memory* memory){
    for (int i = 0; i < memory->max_size; i++){
        if (memory->memory_blocks[i] != NULL){
            free(memory->memory_blocks[i]);
        }
    }
    free(memory->memory_blocks);
    free(memory);
}