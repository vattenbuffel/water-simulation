#include "obstacle_creator.h"
#include <math.h>
#include "circle_creator.h"
#include "board.h"
#include "assert_.h"

//======================================//
// Local function declarations
//======================================//

//======================================//
// Local function implementation
//======================================//

//======================================//
// Global function implementation
//======================================//

void obstacle_init(float *vertices, float side_length, float center_x,
                   float center_y) {
    // Init the four corners
    for (int i = 0; i < 4; i++) {
        double alpha = i * 2 * M_PI / 4 + DEG_TO_RAD(45);
        float x = center_x + side_length / sqrt(2) * cos(alpha)  + RADIUS / 2.f;
        float y = center_y + side_length / sqrt(2) * sin(alpha) + RADIUS / 2.f;
        float z = 0;

        int base_index = i * 3;
        vertices[base_index + 0] = x;
        vertices[base_index + 1] = y;
        vertices[base_index + 2] = z;
    }
}

void obstacles_init(float *vertices, float side_length, float *center_x,
                    float *center_y, int n_obstacles) {
    for (int i = 0; i < n_obstacles; i++) {
        float *cur_pointer = vertices + OBSTACLES_VERTICES_ARRAY_LENGTH(i);
        obstacle_init(cur_pointer, side_length, center_x[i], center_y[i]);
    }
}

void obstacles_connecting_vertices(unsigned int *connections, int n_obstacles) {
    for (int i = 0; i < n_obstacles; i++) {
        int top_right_corner = 4 * i;
        int bottom_left_corner = top_right_corner + 2;
        for (int j = 0; j < 2; j++) {
            int base_index = OBSTACLES_INDEX_ARRAY_LENGTH(i) + j * 3;
            connections[base_index] = top_right_corner;
            connections[base_index + 1] = bottom_left_corner;
            connections[base_index + 2] = top_right_corner + j * 2 + 1;
        }
    }
}


void obstacles_from_grid(Cell *grid, float *vertices, int n_obstacles) {
    // Idiot check to make sure that the number of obstacles is at least within
    // what's possible
    assert_(n_obstacles <= NX * NY && n_obstacles >= 0,
            "n_obstacles must be between 0 and NX*NY");

    int center_i = 0;
    float center_x[n_obstacles];
    float center_y[n_obstacles];
    for (int i = 0; i < NX * NY; i++) {
        state_type state = grid[i].state;
        if (state == states_obstacle) {
            center_x[center_i] = INDEX_TO_X(i);
            center_y[center_i] = INDEX_TO_Y(i);
            center_i++;
        }
    }

    obstacles_init(vertices, OBSTACLE_SIDE_LENGTH, center_x, center_y,
                   n_obstacles);
}
