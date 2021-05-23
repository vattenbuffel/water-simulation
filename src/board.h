#ifndef BOARD_H
#define BOARD_H

#include "min_max.h"
#include "constants.h"

//======================================//
// Defines
//======================================//

// Number of cols
#define NX 100

// Number of rows
#define NY 100

// Macro to calculate the index of position x,y in the grid
#define INDEX_OF_POS(x, y) (y * NX + x)

// Macro to go from index in grid to col
#define INDEX_TO_COL(i) (i % NX)

// Macro to go from index in grid to row
#define INDEX_TO_ROW(i) ((int)i / NX)

// Macro to go from pixel x to grid x.
#define PIXEL_X_TO_GRID_X(x) (MIN(NX-1, round((x) * (float)NX / SCR_WIDTH)))

// Macro to go from pixel y to grid y. 
#define PIXEL_Y_TO_GRID_Y(y) (MIN(NY-1, round((y) * (float)NY / SCR_HEIGHT)))

// Macro to calculate the x pixel value of col x in grid
#define COL_TO_PIXEL_X(x) (x * (float)SCR_WIDTH / NX)

// Macro to calculate the y pixel value of row y in grid
#define ROW_TO_PIXEL_Y(y) (y * (float)SCR_HEIGHT / NY)

// Macro to go from index in grid to x pixel
#define INDEX_TO_X(i) (COL_TO_PIXEL_X(INDEX_TO_COL(i)))

// Macro to go from index in grid to y pixel
#define INDEX_TO_Y(i) (ROW_TO_PIXEL_Y(INDEX_TO_ROW(i)))

// Macro to convert from GLFW's whacky coordinate frame to grid index
#define GLFW_POS_TO_GRID_INDEX(x, y)                                           \
    (INDEX_OF_POS(PIXEL_X_TO_GRID_X(x), PIXEL_Y_TO_GRID_Y(SCR_HEIGHT - y)))

enum States {
    states_background,
    states_water,
    states_obstacle,
    states_max_val
};



typedef struct Board Board;
typedef int state_type;

struct Board{
    // These store the states of the board
    int new_grid[NY * NX];
    // These store the states of the board
    //static int old_grid[NY * NX] = {0};
    int n_circles;
    int n_obstacles;
};

//======================================//
// Global function declarations
//======================================//

/**
 * @brief 
 * 
 * @param board 
 */
void board_init(Board *board);

/**
 * @brief 
 * 
 * @param board 
 */
void board_restart(Board *board);

/**
 * @brief 
 * 
 * @param grid 
 * @param vertices 
 * @param n_circles 
 */
void board_circle_from_grid(int *grid, float *vertices, int n_circles);

/**
 * @brief 
 * 
 * @param grid 
 * @param vertices 
 * @param n_obstacles 
 */
void board_obstacles_from_grid(int *grid, float *vertices, int n_obstacles);

/**
 * @brief 
 * 
 * @param board 
 * @param x 
 * @param y 
 * @param resulting_state 
 */
void board_modify_grid(Board *board, int x, int y, int resulting_state);


#endif //BOARD_H