#ifdef  _CUSTOMER_H
#define _CUSTOMER_H

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
#include "typical.h"

int random_haircut_time(int F);
void customer(int F, int P);

#endif