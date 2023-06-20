#pragma once
#include <stdbool.h>


typedef struct {
    char *foreground;
    char *background;
    int i;
    int j;
} Arguments;


char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);

