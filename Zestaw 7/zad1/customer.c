#include "customer.h"

int random_haircut_time(int F)
{
   return rand() % F;
}


void customer(int F, int P)
{
    int last = 0;
    int *time_table;
    while (working)
    {   //spi od 0 do 4 sekund
        sleep(rand() % 5);
        //losuje fryzure
        int haircut_no = rand() % F;
        int time = resources.time_per_haircut[haircut_no];

        printf("Nowy klient. Fryzura: %d o czasie: %fs\n", haircut_no + 1, time / 1000.0);
        decreaseTimesSem();
        //sprawdzam czy jest miejsce w kolejce
        time_table = shmat(resources.shmid, NULL, 0);
        if (time_table[last + 1] == -1)
        {
            time_table[last + 1] = time;
            last = (last + 1) % P;
            increaseQueueSem();
            printf("Klient siada w kolejce\n");
        }
        else
        {
            printf("Brak miejsc, klient wychodzi\n");
        }
        shmdt(time_table);
        increaseTimesSem();
    }
}