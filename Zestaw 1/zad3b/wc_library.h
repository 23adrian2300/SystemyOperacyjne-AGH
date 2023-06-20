#ifndef WC_LIBRARY_H
#define WC_LIBRARY_H


typedef struct wc_memory{
    char** memory_blocks;
    int max_size;
    int current_size;
} wc_memory;

// inicjalizacja pamieci
wc_memory* wc_memory_init(int max_size);

// funkcja zliczajaca slowa, znaki i linie w pliku
void counter(wc_memory* memory, char* file_name);

//funkcja zwracajaca zawartosc bloku o podanym indeksie
char* block_content_from_index(wc_memory* memory, int index);

//funkcja usuwajaca blok o podanym indeksie
void remove_block_from_index(wc_memory* memory, int index);

//funkcja usuwajaca pamiec
void free_memory(wc_memory* memory);

#endif
