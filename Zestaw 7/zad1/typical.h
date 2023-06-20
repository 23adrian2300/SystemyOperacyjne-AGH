#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>


#define SEMAPHORE_CHAIRS_ID 0
#define SEMAPHORE_QUEUE_ID 1
#define SEMAPHORE_TIMES_ID 2

struct resources{
    int *time_per_haircut;
    int semid, shmid;
} resources;


bool working = true;
struct sembuf sembuff;

// dla ulatwienia
void decreaseChairsSem(){
        sembuff.sem_num = SEMAPHORE_CHAIRS_ID;
        sembuff.sem_op = -1;
        semop(resources.semid, &sembuff, 1);
}

void decreaseQueueSem(){
        sembuff.sem_num = SEMAPHORE_QUEUE_ID;
        sembuff.sem_op = -1;
        semop(resources.semid, &sembuff, 1);
}

void decreaseTimesSem(){
        sembuff.sem_num = SEMAPHORE_TIMES_ID;
        sembuff.sem_op = -1;
        semop(resources.semid, &sembuff, 1);
}

void increaseChairsSem(){
        sembuff.sem_num = SEMAPHORE_CHAIRS_ID;
        sembuff.sem_op = 1;
        semop(resources.semid, &sembuff, 1);
}
void increaseQueueSem(){
        sembuff.sem_num = SEMAPHORE_QUEUE_ID;
        sembuff.sem_op = 1;
        semop(resources.semid, &sembuff, 1);
}
void increaseTimesSem(){
        sembuff.sem_num = SEMAPHORE_TIMES_ID;
        sembuff.sem_op = 1;
        semop(resources.semid, &sembuff, 1);
}