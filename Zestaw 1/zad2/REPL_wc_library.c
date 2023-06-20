#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>
#include <dlfcn.h>
#include "wc_library.h"
#define MAX_COMMAND_LENGTH 512

//enum z komendami
typedef enum {
    INIT,
    COUNT,
    SHOW,
    DELETE,
    DESTROY,
    UNKNOWN,
    EXIT
} commands_enum;

//przypisanie komend do enuma
commands_enum get_command(char *command, char *main_argument){  
    if (strcmp(command, "init") == 0)
    {
        return INIT;
    }
    else if (strcmp(command, "count") == 0)
    {
        return COUNT;
    }
    else if (strcmp(command, "show") == 0)
    {
        return SHOW;
    }
    else if (strcmp(command, "delete") == 0 && main_argument!= NULL && strcmp(main_argument, "index") == 0)
    {
        return DELETE;
    }
    else if (strcmp(command, "destroy") == 0)
    {
        return DESTROY;
    }
    else if (strcmp(command, "exit") == 0)
    {
        return EXIT;
    }
    else
    {
        return UNKNOWN;
    }
}
// wypisywanie uzyskanych czasow
void print_all_time(struct timespec time_start, struct timespec time_end, struct tms tms_start, struct tms tms_end) {
    
    // zmienne do przechowywania czasow
    double real_time, user_time, system_time;
    long clock_ticks = sysconf(_SC_CLK_TCK); //liczba taktow zegara

    //sprawdzenie moliwych bledow
    if (clock_ticks <= 0) {
        printf("Error: sysconf(_SC_CLK_TCK) returned an invalid value\n");
        return;
    }
    //sprawdzenie moliwych bledow
    if (time_end.tv_sec < time_start.tv_sec || (time_end.tv_sec == time_start.tv_sec && time_end.tv_nsec < time_start.tv_nsec)) {
        printf("Error: end time is earlier than start time\n");
        return;
    }
    // czas rzeczywisty
    real_time = (time_end.tv_sec - time_start.tv_sec) + (time_end.tv_nsec - time_start.tv_nsec) / 1000000000.0;

    // czas uzytkownika
    user_time = (tms_end.tms_utime - tms_start.tms_utime) / (double) clock_ticks;

    // czas systemowy
    system_time = (tms_end.tms_stime - tms_start.tms_stime) / (double) clock_ticks;

    // wypisanie czasow
    printf("Real time: %.5lfs User time: %.5lfs System time: %.5lfs \n", real_time, user_time, system_time);
}



int main(int argc, char **argv){
// wczytywanie biblioteki
#ifdef DLL
  void *handle = dlopen("./libwc_library.so", RTLD_LAZY);

  // wczytywanie funkcji z biblioteki
  wc_memory *(*wc_memory_init)(int) = dlsym(handle, "wc_memory_init");
  void (*counter)(wc_memory *, char *) = dlsym(handle, "counter");
  char *(*block_content_from_index)(wc_memory *, int) = dlsym(handle, "block_content_from_index");
  void (*remove_block_from_index)(wc_memory *, int) = dlsym(handle, "remove_block_from_index");
  void (*free_memory)(wc_memory *) = dlsym(handle, "free_memory");

  // sprawdzanie czy wczytanie sie powiodlo
  if (handle == NULL)
  {
      printf("libwc_library.so not found!\n");
      exit(0);
  }
#endif

//Komunikaty o błędach i dzialaniu
const char initialized_problem[] = "Wc_memory is not initialized!\n";
const char size_problem[] = "Size has to be greater than 0!\n";
const char no_size_problem[] = "Size must be specified!\n";
const char element_problem[] = "There is no element at this index!\n";
const char unknown_command_problem[] = "This is unknown command! \n";
const char exiting_repl[] = "Exiting... \n";


// wskaznik na pamiec
wc_memory *memory = NULL; 

// bufor do wczytywania komendy
char buffer[MAX_COMMAND_LENGTH];

// zmienne do przechowywania komendy i argumentow
char *command;
char *main_argument;
// secondary_argument używany tylko przy usuwaniu elementu z podanego indeksu
char *secondary_argument;

// flaga do wyjscia z petli
int flag = 1;

// komunikat powitalny
printf("Welcome to REPL!\n\n");

// zmienne do pomiaru czasu
struct timespec time_start, time_end;
struct tms tms_start,tms_end;

// petla REPL
  while (flag ==1)
  {   
    // Wczytywanie komendy
    fgets(buffer, MAX_COMMAND_LENGTH, stdin);

    // Usuwanie znaku nowej linii z bufora
    buffer[strcspn(buffer, "\n")] = 0;

    // Rozdzielenie komendy na komende i argumenty
    command = strtok(buffer, " ");
    main_argument= strtok(NULL, " ");
    secondary_argument = strtok(NULL, " ");

    // wyświetlanie komendy (przydatne w raportach)
    switch (get_command(command, main_argument)){   
        case INIT:
            printf("[%s %s]: ", command,main_argument);
            break;
        case COUNT:
            printf("[%s %s]: ", command,main_argument);
            break;
        case SHOW:
            printf("[%s %s]: ", command,main_argument);   
            break;     
        case DELETE:
            printf("[%s %s %s]: ", command,main_argument,secondary_argument);
            break;
        case DESTROY:
            printf("[%s]: ", command);
            break;
        case EXIT:
            printf("[%s]: ", command);
            break;
        case UNKNOWN:
            break;
            }  
     
      // Wykonanie komendy
        switch (get_command(command, main_argument)){
            case INIT:
                if (memory != NULL)
                {
                    printf(initialized_problem);
                    continue;
                }
                if (main_argument == NULL)
                {
                    printf(no_size_problem);
                    continue;
                }

                int size = atoi(main_argument);

                if (size <= 0)
                {
                    printf(size_problem);
                    continue;
                }

                // Pomiary czasu
                times(&tms_start);

                // CLOCK_REALTIME jest zglaszane jako blad natomiast wszytsko sie kompiluje i dziala
                clock_gettime(CLOCK_REALTIME, &time_start); 
                
                
                // Inicjalizacja pamieci
                memory = wc_memory_init(size);
        
                clock_gettime(CLOCK_REALTIME, &time_end);
                times(&tms_end);

                // Wypisanie czasu
                print_all_time(time_start, time_end, tms_start, tms_end);
                continue;

            case COUNT:
                if (memory == NULL)
                {
                    printf(initialized_problem);
                    continue;
                }

                // Pomiary czasu
                times(&tms_start);
                clock_gettime(CLOCK_REALTIME, &time_start);

                // Wykonanie funkcji
                counter(memory, main_argument);

                clock_gettime(CLOCK_REALTIME, &time_end);
                times(&tms_end);

                // Wypisanie czasu
                print_all_time(time_start, time_end, tms_start, tms_end);
                continue;

            case SHOW:
                if (memory == NULL)
                {
                    printf(initialized_problem);
                    continue;
                }

                // Pomiary czasu
                times(&tms_start);
                clock_gettime(CLOCK_REALTIME, &time_start);

                // Wykonanie funkcji
                char *element = block_content_from_index(memory, atoi(main_argument));
                if (element == NULL){
                    printf(element_problem);
                    continue;
                }

                // Wypisanie wyniku
                printf("%s ", element);
                
                clock_gettime(CLOCK_REALTIME, &time_end);
                times(&tms_end);

                // Wypisanie czasu
                print_all_time(time_start, time_end, tms_start, tms_end);
                continue;

            case DELETE:
                if (memory == NULL)
                {
                    printf(initialized_problem);
                    continue;
                }
                // Pomiary czasu
                times(&tms_start);
                clock_gettime(CLOCK_REALTIME, &time_start);

                // Wykonanie funkcji
                remove_block_from_index(memory, atoi(secondary_argument));
                
                clock_gettime(CLOCK_REALTIME, &time_end);
                times(&tms_end);

                // Wypisanie czasu
                print_all_time(time_start, time_end, tms_start, tms_end);
                continue;

            case DESTROY:
                if (memory == NULL)
                {
                    printf(initialized_problem);
                    continue;
                }
                // Pomiary czasu
                times(&tms_start);
                clock_gettime(CLOCK_REALTIME, &time_start);

                // Wykonanie funkcji
                free_memory(memory);
                memory = NULL;

                clock_gettime(CLOCK_REALTIME, &time_end);
                times(&tms_end);

                // Wypisanie czasu
                print_all_time(time_start, time_end, tms_start, tms_end);
                continue;

            case UNKNOWN:
                printf(unknown_command_problem);
                continue;  

            case EXIT:
                printf(exiting_repl);

                // Zakonczenie petli REPL
                flag = 0;       
        }
      
  }
#ifdef DLL
// Zamkniecie biblioteki
  dlclose(handle);
#endif
  return 0;
}