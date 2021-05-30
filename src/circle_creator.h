#ifndef CIRCLE_CREATOR_H
#define CRICLE_CREATOR_H

#include "board.h"

//======================================//
// Defines
//======================================//

// Radius of the water circles
// #define RADIUS (2 * SCR_WIDTH / NX)
#define RADIUS (1 * SCR_WIDTH / NY)

// How many triangle each cricle consists of
#define N_TRIANGLES 5

// Macro to calculate how long an array needs to be to store both the vertices
// of water circles and it's mass
#define CIRCLE_VERTICES_ARRAY_LENGTH (N_TRIANGLES * 3 + 3 + N_TRIANGLES + 1)

// Macro to calculate how long the array needs to be to store multiple circles
#define CIRCLES_VERTICES_ARRAY_LENGTH(n_circles)                               \
    (CIRCLE_VERTICES_ARRAY_LENGTH * (n_circles))

// How long an array needs to be to store a single circle index
#define CIRCLE_INDEX_ARRAY_LENGTH (N_TRIANGLES * 3)

// Macro to calculate how long an array needs to be to store a multiple circle
// index
#define CIRCLES_INDEX_ARRAY_LENGTH(n_circles)                                  \
    (CIRCLE_INDEX_ARRAY_LENGTH * (n_circles))

// How many vertices in one circle
#define CIRCLE_VERTICES_IN_CIRCLE (N_TRIANGLES + 1)

struct CircleVertex {
    float pos[3];
    float mass;
};
typedef struct CircleVertex CircleVertex;

struct Circle{
    CircleVertex vertices[CIRCLE_VERTICES_IN_CIRCLE];

};
typedef struct Circle Circle;

struct CircleAll{
    Circle circles[NX*NY];
};
typedef struct CircleAll CircleAll;

//======================================//
// Global function declarations
//======================================//

/**
 * @brief
 *
 * @param circle
 * @param radius
 * @param center_x
 * @param center_y
 */
void circle_populate_circle(Circle *circle, float radius, float center_x,
                          float center_y);

/**
 * @brief
 *
 * @param vertices
 * @param radius
 * @param center_x
 * @param center_y
 * @param n_triangles
 */
void circle_populate_circles(CircleAll *circles, float radius, float *center_x,
                           float *center_y, int n_triangles);

/**
 * @brief
 *
 * @param connections
 * @param n_triangles
 */
void circle_connecting_vertices(unsigned int *connections, int n_triangles);

/**
 * @brief
 *
 * @param grid
 * @param vertices
 * @param n_circles
 */
void circle_from_grid(Cell *grid, CircleAll* circles, int n_circles);

#endif /*CIRCLE_CREATOR_H*/