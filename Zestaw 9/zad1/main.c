#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

// inicjalizacja
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_of_santa = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_of_elves = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_of_reindeers = PTHREAD_COND_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_waiting_mutex = PTHREAD_MUTEX_INITIALIZER;

// zmienne
int elves_queue[3];
int number_of_reindeers= 0;
int number_of_elves = 0;
bool working = true;
int let_reindeers_go = 1;

//funkcja elfow
void* elves_function(void* arg) {
    while (true) {
        usleep((rand()%3000+2000)*1000);
        //blokada mutexa
        pthread_mutex_lock(&mutex);
        number_of_elves++;
        if (number_of_elves < 3) {
            elves_queue[number_of_elves-1] = *((int*)arg);
            printf("[ELF]: czeka %d elfow na Mikolaja, ID: %d\n", number_of_elves, *((int*)arg));
            //czekanie na sygnal
            pthread_cond_wait(&cond_of_elves, &mutex);
        } else if (number_of_elves == 3) {
            elves_queue[2] = *((int*)arg);
            printf("[ELF]: czeka %d elfow na Mikolaja, ID: %d\n", 3, *((int*)arg));
            printf("[ELF]: wybudzam Mikolaja, ID: %d\n", *((int*)arg));
            //sygnalizacja
            pthread_cond_signal(&cond_of_santa);
        } else {
            printf("[ELF]: samodzielnie rozwiazuje problem, ID: %d\n", *((int*)arg));
        }
        //odblokowanie mutexa
        pthread_mutex_unlock(&mutex);
    }
}


void* reindeers_function(void* arg) {
    int index = *((int *) arg);
    while (true) {
        //blokada mutexa
        pthread_mutex_lock(&reindeer_waiting_mutex);
        while (!let_reindeers_go) {
            //czekanie na sygnal
            pthread_cond_wait(&cond_of_reindeers, &reindeer_waiting_mutex);
        }
        //odblokowanie mutexa
        pthread_mutex_unlock(&reindeer_waiting_mutex);
        usleep((rand()%5000+5000)*1000);
        //blokada mutexa
        pthread_mutex_lock(&reindeer_mutex);
        number_of_reindeers++;
        printf("[Renifer]: czeka %d reniferow na Mikolaja, ID: %d\n", number_of_reindeers, index);
        let_reindeers_go = 0;
        if (number_of_reindeers == 9) {
            printf("[Renifer]: wybudzam Mikolaja, ID: %d\n", index);
            //sygnalizacja
            pthread_mutex_lock(&mutex);
            pthread_cond_broadcast(&cond_of_santa);
            pthread_mutex_unlock(&mutex);

        }
        //odblokowanie mutexa
        pthread_mutex_unlock(&reindeer_mutex);
    }
}

void* santa_function(void* args) {
    // rozdawanie prezentow
    int time_for_presents = 0;
    while (true) {
        //blokada mutexa
        pthread_mutex_lock(&mutex);
        if (number_of_elves >= 3) {
            printf("[Mikolaj]: rozwiązuje problemy elfow %d %d %d\n", elves_queue[0], elves_queue[1], elves_queue[2]);
            usleep((rand()%1000+1000)*1000);
            number_of_elves = 0;
            //sygnalizacja
            pthread_cond_broadcast(&cond_of_elves);
        }
        if (number_of_reindeers== 9) {
            printf("[Mikolaj]: dostarczam zabawki\n");
            number_of_reindeers = 0;
            time_for_presents++;
            usleep((rand()%2000+2000)*1000);
            let_reindeers_go = 1;
            //sygnalizacja
            pthread_cond_broadcast(&cond_of_reindeers);

        }
        printf("[Mikolaj]: zasypiam\n");
        
        if (time_for_presents== 3) {
            working = false;
            //odblokowanie mutexa
            pthread_mutex_unlock(&mutex);
            printf("[Mikolaj]: koncze prace\n");
            break;
        }
        //czekanie na sygnal
        pthread_cond_wait(&cond_of_santa, &mutex);
        printf("[Mikolaj]: budze sie\n");
        //odblokowanie mutexa
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
//funkcja niszczenia
void destroyer(){
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_of_santa);
    pthread_cond_destroy(&cond_of_elves);
    pthread_cond_destroy(&cond_of_reindeers);
    pthread_mutex_destroy(&reindeer_mutex);
    pthread_mutex_destroy(&reindeer_waiting_mutex);
}

int main() {
    srand(time(NULL));
    pthread_t santa_thread;
    pthread_t elves_threads[10];
    pthread_t reindeers_threads[9];

    pthread_create(&santa_thread, NULL, santa_function, NULL);

    for (int i = 0; i < 10; i++) {
        int* index = malloc(sizeof(int));
        *index = i + 1;
        pthread_create(&elves_threads[i], NULL, elves_function, index);
    }

    for (int i = 0; i < 9; i++) {
        int* index = malloc(sizeof(int));
        *index = i + 1;
        pthread_create(&reindeers_threads[i], NULL, reindeers_function, index);
    }

    while (working);

    for (int i = 0; i < 10; i++) {
        pthread_cancel(elves_threads[i]);
    }
    pthread_cancel(santa_thread);
    for (int i = 0; i < 9; i++) {
        pthread_cancel(reindeers_threads[i]);
    }
    
    destroyer();
    exit(EXIT_SUCCESS);
}



