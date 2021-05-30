#ifndef OBSTACLE_CREATOR_H
#define OBSTACLE_CREATOR_H

#include "board.h"

//======================================//
// Defines
//======================================//

// How long an array must be to contain the vertices of 1 obstacle
#define OBSTACLE_VERTICES_ARRAY_LENGTH 12

// How long an array must be to contain the vertices of n obstacles
#define OBSTACLES_VERTICES_ARRAY_LENGTH(n) (n * OBSTACLE_VERTICES_ARRAY_LENGTH)

// How long an array must be to contain the index of 1 obstacle
#define OBSTACLE_INDEX_ARRAY_LENGTH 6

// How long an array must be to contain the index of n obstacles
#define OBSTACLES_INDEX_ARRAY_LENGTH(n) (n * OBSTACLE_INDEX_ARRAY_LENGTH)

#define DEG_TO_RAD(deg) (deg / 180.f * M_PI)

//======================================//
// Global function declarations
//======================================//
/**
 * @brief
 *
 * @param vertices
 * @param side_length
 * @param center_x
 * @param center_y
 */
void obstacle_init(float *vertices, float side_length, float center_x,
                   float center_y);

/**
 * @brief
 *
 * @param vertices
 * @param side_length
 * @param center_x
 * @param center_y
 * @param n_obstacles
 */
void obstacles_init(float *vertices, float side_length, float *center_x,
                    float *center_y, int n_obstacles);

/**
 * @brief
 *
 * @param connections
 * @param n_triangles
 */
void obstacles_connecting_vertices(unsigned int *connections, int n_triangles);

/**
 * @brief
 *
 * @param grid
 * @param vertices
 * @param n_obstacles
 */
void obstacles_from_grid(Cell *grid, float *vertices, int n_obstacles);


#endif /*OBSTACLE_CREATOR_H*/