#ifndef BOARD_H
#define BOARD_H

#include "constants.h"
#include "min_max.h"

//======================================//
// Defines
//======================================//

// Number of cols
#define NX 100

// Number of rows
#define NY 100

// Macro to calculate the index of position x,y in the grid
#define INDEX_OF_POS(x, y) ((y)*NX + x)

// Macro to go from index in grid to col
#define INDEX_TO_COL(i) ((i) % NX)

// Macro to go from index in grid to row
#define INDEX_TO_ROW(i) ((int)(i) / NX)

// Macro to go from pixel x to grid x.
#define PIXEL_X_TO_GRID_X(x) (MIN(NX - 1, round((x) * (float)NX / SCR_WIDTH)))

// Macro to go from pixel y to grid y.
#define PIXEL_Y_TO_GRID_Y(y) (MIN(NY - 1, round((y) * (float)NY / SCR_HEIGHT)))

// Macro to calculate the x pixel value of col x in grid
#define COL_TO_PIXEL_X(x) ((x) * (float)SCR_WIDTH / NX)

// Macro to calculate the y pixel value of row y in grid
#define ROW_TO_PIXEL_Y(y) ((y) * (float)SCR_HEIGHT / NY)

// Macro to go from index in grid to x pixel
#define INDEX_TO_X(i) (COL_TO_PIXEL_X(INDEX_TO_COL(i)))

// Macro to go from index in grid to y pixel
#define INDEX_TO_Y(i) (ROW_TO_PIXEL_Y(INDEX_TO_ROW(i)))

// Macro to convert from GLFW's whacky coordinate frame to grid index
#define GLFW_POS_TO_GRID_INDEX(x, y)                                           \
    (INDEX_OF_POS(PIXEL_X_TO_GRID_X((x)), PIXEL_Y_TO_GRID_Y(SCR_HEIGHT - (y))))

// How much mass of water a cell can max have
#define BOARD_MAX_MASS 1.0

// How much mass a water cell starts with
#define BOARD_START_MASS BOARD_MAX_MASS

// How much excess water a cell can have
#define BOARD_EXCESS_MASS 0.02

// Ignore the cells which have less than this amount of mass
#define BOARD_MIN_MASS 0.0001

// Max flow per frame
#define BOARD_MAX_FLOW 0.1

// Min flow per frame
#define BOARD_MIN_FLOW BOARD_MIN_MASS

// MACRO TO LIMIT FLOW
#define BOARD_LIMIT_FLOW(flow) (MIN((flow), BOARD_MAX_FLOW))

enum States {
    states_background,
    states_water,
    states_obstacle,
    states_max_val
};

typedef int state_type;

typedef struct Cell Cell;

struct Cell {
    float mass;
    // float mass_max;
    state_type state;
};

typedef struct Board Board;

struct Board {
    // These store the states of the board
    Cell grid[NY * NX];
    int n_circles;
    int n_obstacles;
    // Water mass in order from top left to bottom right. Index 0 is the mass of
    // the first water cell, index 1 of second water etc.
    float mass_in_order[NY * NX];
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
 * @param n_obstacles
 */
void board_obstacles_from_grid(Cell *grid, float *vertices, int n_obstacles);

/**
 * @brief
 *
 * @param board
 * @param x
 * @param y
 * @param resulting_state
 */
void board_modify_grid(Board *board, int x, int y, int resulting_state);

/**
 * @brief https://w-shadow.com/blog/2009/09/01/simple-fluid-simulation/
 *
 * @param board
 */
void board_simulate(Board *board, Board *new_board);

#endif // BOARD_H