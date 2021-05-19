#ifndef CIRCLE_CREATOR_H
#define CRICLE_CREATOR_H

//======================================//
// Defines
//======================================//
// How many triangle each cricle consists of
#define N_TRIANGLES 50

// How long an array needs to be to store a single circle
#define CIRCLE_VERTICES_ARRAY_LENGTH (N_TRIANGLES*3 + 3)

// Macro to calculate how long the array needs to be to store multiple circles
#define CIRCLES_VERTICES_ARRAY_LENGTH(n_circles) (CIRCLE_VERTICES_ARRAY_LENGTH*n_circles)

// How long an array needs to be to store a single circle index
#define CIRCLE_INDEX_ARRAY_LENGTH (N_TRIANGLES*3)

// Macro to calculate how long an array needs to be to store a multiple circle index
#define CIRCLES_INDEX_ARRAY_LENGTH(n_circles) (CIRCLE_INDEX_ARRAY_LENGTH*n_circles)



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
void init_triangle_circle(float* vertices, float radius, float center_x, float center_y);

/**
 * @brief 
 * 
 * @param vertices 
 * @param radius 
 * @param center_x 
 * @param center_y 
 * @param n_triangles 
 */
void init_triangle_circles(float* vertices, float radius, float *center_x, float *center_y, int n_triangles);

/**
 * @brief 
 * 
 * @param connections 
 * @param n_triangles 
 */
void circles_connecting_vertices(unsigned int* connections, int n_triangles);

#endif /*CIRCLE_CREATOR_H*/