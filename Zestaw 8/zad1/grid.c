#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>


const int grid_width = 30;
const int grid_height = 30;


char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}


void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }


    refresh();
}


void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}


bool is_alive(int row, int col, char *grid)
{
    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}
void handle_ignore(int signum) {} 


void* single_thread(void* args) {
    Arguments* arguments = (Arguments*) args;
    int i = arguments->i;
    int j = arguments->j;
    while(1) {
        arguments->background[i * grid_width + j] = is_alive(i, j, arguments->foreground);
        pause();
        char* tmp = arguments->foreground;
        arguments->foreground = arguments->background;
        arguments->background = tmp;
    }
}

void update_grid(char *foreground, char *background)
{   static pthread_t* threads = NULL;
    
    if (!threads) {
        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = handle_ignore;
        sigaction(SIGUSR1, &action, NULL);
        threads = malloc(sizeof(pthread_t) * grid_width * grid_height);

        for (int i = 0; i < grid_height; i++) {
    for (int j = 0; j < grid_width; j++) {
        Arguments* args = malloc(sizeof(Arguments));
        args->foreground = foreground;
        args->background = background;
        args->i = i;
        args->j = j;
        pthread_create(&threads[i * grid_width + j], NULL, single_thread, (void*) args);
    }
}


    }
    for(int i = 0; i < grid_width * grid_height; i++) {
        pthread_kill(threads[i], SIGUSR1);
    }
}




