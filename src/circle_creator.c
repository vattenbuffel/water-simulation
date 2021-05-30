#include "circle_creator.h"
#include "constants.h"
#include <math.h>
#include <stdio.h>
// #include <assert.h>
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

void circle_populate_circle(Circle *circle, float radius, float center_x,
                          float center_y) {
    circle->vertices[0].pos[0] = center_x + RADIUS/2.f;
    circle->vertices[0].pos[1] = center_y + RADIUS/2.f;
    circle->vertices[0].pos[2] = 0;

    // Calculate the values of the vertices
    for (int i = 1; i < CIRCLE_VERTICES_IN_CIRCLE; i++) {
        double alpha = 2 * M_PI / N_TRIANGLES * i;
        float x = radius * cos(alpha) + center_x  + RADIUS/2.f;
        float y = radius * sin(alpha) + center_y  + RADIUS/2.f;
        circle->vertices[i].pos[0] = x;
        circle->vertices[i].pos[1] = y;
        circle->vertices[i].pos[2] = 0;
    }
}

void circle_populate_circles(CircleAll *circles, float radius, float *center_x,
                           float *center_y, int n_circles) {
    // printf("In init_triangle_circles n_circles: %d\n", n_circles);
    // Idiot check to make sure that the number of circles is at least within
    // what's possible
    assert_(n_circles <= NX * NY && n_circles >= 0,
            "n_circles must be between 0 and NX*NY");

    // Create the triangle circles
    for (int i = 0; i < n_circles; i++) {
        Circle *circle = &(circles->circles[i]);
        circle_populate_circle(circle, radius, center_x[i], center_y[i]);
    }
}

void circle_from_grid(Cell *grid, CircleAll* circles, int n_circles) {
    // printf("In circle_from_grid n_circles: %d\n", n_circles);
    // Idiot check to make sure that the number of circles is at least within
    // what's possible
    assert_(n_circles <= NX * NY && n_circles >= 0,
            "n_circles must be between 0 and NX*NY");

    int center_i = 0;
    float center_x[n_circles];
    float center_y[n_circles];
    for (int i = 0; i < NX * NY; i++) {
        state_type state = grid[i].state;
        if (state == states_water) {
            center_x[center_i] = INDEX_TO_X(i);
            center_y[center_i] = INDEX_TO_Y(i);
            center_i++;
        }
    }

    circle_populate_circles(circles, RADIUS, center_x, center_y, n_circles);
}

void circle_connecting_vertices(unsigned int *connections, int n_triangles) {
    for (int i = 0; i < n_triangles; i++) {
        for (int j = 0; j < N_TRIANGLES; j++) {
            int base_index = i * CIRCLE_INDEX_ARRAY_LENGTH + j * 3;

            connections[base_index + 0] = 0 + i * (N_TRIANGLES + 1);
            connections[base_index + 1] = 1 + j + i * (N_TRIANGLES + 1);
            if (j + 1 == N_TRIANGLES)
                connections[base_index + 2] = 1 + i * (N_TRIANGLES + 1);
            else
                connections[base_index + 2] = 2 + j + i * (N_TRIANGLES + 1);
        }
    }
}
