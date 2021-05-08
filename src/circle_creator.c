#include "circle_creator.h"
#include <math.h>

//======================================//
// Global function declarations
//======================================//

//======================================//
// Local function declarations
//======================================//

//======================================//
// Global function declarations
//======================================//

void init_triangle_circle(float *vertices, float radius, float center_x,
                          float center_y) {
    vertices[0] = center_x;
    vertices[1] = center_y;
    vertices[2] = 0;

    // Calculate the values of the vertices
    for (int i = 0; i < N_TRIANGLES; i++) {
        float alpha = 2 * M_PI / N_TRIANGLES * i;
        float x = radius * cos((double)alpha) + center_x;
        float y = radius * sin(alpha) + center_y;
        vertices[i * 3 + 0 + 3] = x;
        vertices[i * 3 + 1 + 3] = y;
        vertices[i * 3 + 2 + 3] = 0;
    }
}

void init_triangle_circles(float *vertices, float radius, float *center_x,
                           float *center_y, int n_triangles) {
    for (int i = 0; i < n_triangles; i++) {
        float *cur_vertices =
            vertices + CIRCLES_VERTICES_ARRAY_LENGTH(i);
        init_triangle_circle(cur_vertices, radius, center_x[i], center_y[i]);
    }
}

void connecting_vertices(int *connections, int n_triangles) {
    for (int i = 0; i < n_triangles; i++) {
        for (int j = 0; j < N_TRIANGLES; j++) {
            int base_index = i * CIRCLE_INDEX_ARRAY_LENGTH + j * 3;

            connections[base_index + 0] = 0 + i * (N_TRIANGLES+1);
            connections[base_index + 1] = 1 + j + i * (N_TRIANGLES+1);
            if (j + 1 == N_TRIANGLES)
                connections[base_index + 2] = 1 + i * (N_TRIANGLES+1);
            else
                connections[base_index + 2] = 2 + j + i * (N_TRIANGLES+1);
        }
    }
}
