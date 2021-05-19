#include "obstacle_creator.h"
#include <math.h>

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
        float x = center_x + side_length / sqrt(2) * cos(alpha);
        float y = center_y + side_length / sqrt(2) * sin(alpha);
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
