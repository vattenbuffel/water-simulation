#include "board.h"
#include "circle_creator.h"
#include "obstacle_creator.h"
#include <stdbool.h>
#include <stdio.h>
#include "assert_.h"

//======================================//
// Local function declarations
//======================================//

//======================================//
// Local function implementations
//======================================//

//======================================//
// Global function implementations
//======================================//

void board_init(Board *board) {
    board->n_circles = 0;
    board->n_obstacles = 0;
    int board_size = sizeof(board->new_grid) / sizeof(board->new_grid[0]);
    for (int i = 0; i < board_size; i++) {
        board->new_grid[i] = 0;
    }
}

void board_circle_from_grid(int *grid, float *vertices, int n_circles) {
    // printf("In circle_from_grid n_circles: %d\n", n_circles);
    // Idiot check to make sure that the number of circles is at least within
    // what's possible
    assert_(n_circles <= NX * NY && n_circles >= 0,
            "n_circles must be between 0 and NX*NY");

    int center_i = 0;
    float center_x[n_circles];
    float center_y[n_circles];
    for (int i = 0; i < NX * NY; i++) {
        int state = grid[i];
        if (state == states_water) {
            center_x[center_i] = INDEX_TO_X(i);
            center_y[center_i] = INDEX_TO_Y(i);
            center_i++;
        }
    }

    init_triangle_circles(vertices, RADIUS, center_x, center_y, n_circles);
}

void board_obstacles_from_grid(int *grid, float *vertices, int n_obstacles) {
    // Idiot check to make sure that the number of obstacles is at least within
    // what's possible
    assert_(n_obstacles <= NX * NY && n_obstacles >= 0,
            "n_obstacles must be between 0 and NX*NY");

    int center_i = 0;
    float center_x[n_obstacles];
    float center_y[n_obstacles];
    for (int i = 0; i < NX * NY; i++) {
        int state = grid[i];
        if (state == states_obstacle) {
            center_x[center_i] = INDEX_TO_X(i);
            center_y[center_i] = INDEX_TO_Y(i);
            center_i++;
        }
    }

    obstacles_init(vertices, OBSTACLE_SIDE_LENGTH, center_x, center_y,
                   n_obstacles);
}

void board_dec_counter(Board *board, state_type state) {
    if (state == states_water)
        board->n_circles -= 1;

    if (state == states_obstacle)
        board->n_obstacles -= 1;
}

void board_inc_counter(Board *board, state_type state) {
    if (state == states_water)
        board->n_circles += 1;

    if (state == states_obstacle)
        board->n_obstacles += 1;
}