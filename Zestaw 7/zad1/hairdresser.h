#ifdef  _HAIRDRESSER_H
#define _HAIRDRESSER_H

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

void hairdresser(int P, int F);

#endif