
#include "typical.h"
#include "hairdresser.c"
#include "customer.c"


void signal_handler(int signum)
{
   working = false;
   exit(0);
}

// sprawdza czy podane argumenty sa poprawne
void validation(int M, int N, int P, int F)
{   
    if (M < N)
   {
       fprintf(stderr, "M musi byc > N\n");
       exit(1);
   }

   if (M < 1 || N < 1 || P < 1 || F < 1)
   {
       fprintf(stderr, "Argumenty musza byc wieksze od 0\n");
       exit(1);
   }
   
}

// inicjalizuje zasoby
void initialize(int N, int M, int P, int F)
{  
    key_t key_semid = ftok(getenv("HOME"),rand() % 512);
    key_t key_shmid = ftok(getenv("HOME"),rand() % 512);    

    if ((resources.semid = semget(key_semid, 3, IPC_CREAT | 0666)) == -1)
    {
        perror("semget");
        exit(1);
    }
    if ((resources.shmid = shmget(key_shmid, (P + 1) * sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        perror("shmget");
        exit(1);
    }
    semctl(resources.semid, SEMAPHORE_CHAIRS_ID, SETVAL, N);
    semctl(resources.semid, SEMAPHORE_QUEUE_ID, SETVAL, 0);
    semctl(resources.semid, SEMAPHORE_TIMES_ID, SETVAL, 1);
    sembuff.sem_flg = 0;
 
}


int main(int argc, char **argv)
{
    if (argc < 5)
    {
        fprintf(stderr, "Podaj M N P F\n");
        exit(1);
    }
    int M = atoi(argv[1]);
    int N = atoi(argv[2]);
    int P = atoi(argv[3]);
    int F = atoi(argv[4]);

    validation(M, N, P, F);
    setbuf(stdout, NULL);
    srand(time(NULL));
    signal(SIGINT, signal_handler);
    
    resources.time_per_haircut = malloc(F * sizeof(int));
    //losuje czas trwania fryzur
    for (int i = 0; i < F; i++){ 
    resources.time_per_haircut[i] = 1000 + random() % 5000;
    }

    //inicjalizuje zasoby
    initialize(M,N,P,F);
    int *time_table = (int *)shmat(resources.shmid, NULL, 0);
    memset(time_table, -1, (P + 1) * sizeof(int));
    time_table[0] = 0;
    shmdt(time_table);
    //tworzy procesy fryzjerow
    int i = 0;
    while (i<M)
    {
        if (fork() == 0)
        {
            hairdresser(i,P);
            exit(0);
        }
        i++;
    }
    //tworzy procesy klientow
    customer(F,P);
    while (wait(NULL) > 0);
    //czysci zasoby
    free(resources.time_per_haircut);
    semctl(resources.semid, 0, IPC_RMID);
    shmdt(time_table);
    shmctl(resources.shmid, IPC_RMID, NULL);
    exit(0);
}













