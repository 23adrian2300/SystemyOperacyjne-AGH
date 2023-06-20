#include "hairdresser.h"

void hairdresser(int id, int P)
{
    int *time_table;
    printf("Fryzjer (id: %d) zaczal prace\n", id);
    
    while (working)
    {
        decreaseQueueSem();
        decreaseChairsSem();

        printf("Fryzjer (id: %d) wybiera klienta do strzyzenia\n", id);

        time_table = shmat(resources.shmid, NULL, 0);

        decreaseTimesSem();

        int haircut_time  = time_table[time_table[0] + 1];
        time_table[time_table[0] + 1] = -1;
        time_table[0] = (time_table[0] + 1) % P;
        
        shmdt(time_table);
        sembuff.sem_op = 1;
        semop(resources.semid, &sembuff, 1);

        usleep(haircut_time );
        printf("Fryzjer (id: %d) skonczyl prace\n", id);

        increaseChairsSem(); 
    }
}
