#include "board.h"
#include "assert_.h"
#include "circle_creator.h"
#include "obstacle_creator.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//======================================//
// Local function declarations
//======================================//
/**
 * @brief
 *
 * @param board
 * @param state
 */
void board_dec_counter(Board *board, state_type state);

/**
 * @brief
 *
 * @param board
 * @param state
 */
void board_inc_counter(Board *board, state_type state);

/**
 * @brief
 *
 * @param cell1
 * @param cell2
 */
void board_cell_merge_vertical(Cell *cell1, Cell *cell2);

/**
 * @brief
 *
 * @param cell1
 * @param cell2
 */
void board_cell_merge_horizontal(Cell *cell1, Cell *cell2);

//======================================//
// Local function implementations
//======================================//
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

//======================================//
// Global function implementations
//======================================//

void board_init(Board *board) {
    // Set all the cells to background
    int board_size = sizeof(board->grid) / sizeof(board->grid[0]);
    for (int i = 0; i < board_size; i++) {
        board_modify_grid(board, INDEX_TO_COL(i), INDEX_TO_ROW(i),
                          states_background);
    }

    board->n_circles = 0;
    board->n_obstacles = 0;
}

void board_restart(Board *board) { board_init(board); }

void board_circle_from_grid(Cell *grid, float *vertices, int n_circles) {
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

    init_triangle_circles(vertices, RADIUS, center_x, center_y, n_circles);
}

void board_obstacles_from_grid(Cell *grid, float *vertices, int n_obstacles) {
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

void board_modify_grid(Board *board, int x, int y, int resulting_state) {
    // printf("Gonna modify grid!\n");
    assert_(x <= NX - 1 && x >= 0,
            "Grid_x must be smaller than NX and greater than 0");
    assert_(y <= NY - 1 && y >= 0,
            "Grid_y must be smaller than NY and greater than 0");

    // Update the counters of how many obstacles and waters there ar1e
    int index = INDEX_OF_POS(x, y);
    state_type old_state = board->grid[index].state;
    board_inc_counter(board, resulting_state);
    board_dec_counter(board, old_state);
    board->grid[index].state = resulting_state;
    board->grid[index].mass =
        resulting_state == states_water ? BOARD_START_MASS : 0;
}

// Returns the amount of water that should be in the bottom cell.
float get_stable_state_b(float total_mass) {
    if (total_mass <= 1) {
        return 1;
    } else if (total_mass < 2 * BOARD_MAX_MASS + BOARD_EXCESS_MASS) {
        return (BOARD_MAX_MASS * BOARD_MAX_MASS +
                total_mass * BOARD_EXCESS_MASS) /
               (BOARD_MAX_MASS + BOARD_EXCESS_MASS);
    } else {
        return (total_mass + BOARD_EXCESS_MASS) / 2;
    }
}

void board_simulate(Board *board, Board *new_board) {
    // printf("Gonna simluate water!!!\n");

    // Restart the new_board
    board_restart(new_board);

    // Start simulating from the top
    for (int y = NY - 1; y >= 0; y--) {
        for (int x = 0; x < NX; x++) {
            int cur_index = INDEX_OF_POS(x, y);
            Cell cur_cell = board->grid[cur_index];

            // Only water cells have any simulation
            if (cur_cell.state != states_water)
                continue;

            // Step 1) Move water down
            float cur_mass = cur_cell.mass;
            int index_below = INDEX_OF_POS(x, y - 1);
            Cell cell_below = board->grid[index_below];

            // Only do something if cell below is not obstacle and this cell
            // isn't is at the bottom
            if (cell_below.state != states_obstacle && y != 0) {
                float mass_below = cell_below.mass;
                float possible_mass_below =
                    get_stable_state_b(cur_mass + mass_below);
                float mass_to_move = possible_mass_below - mass_below;
                float flow = MAX(cur_mass, mass_to_move);
                flow = BOARD_LIMIT_FLOW(flow);
                cur_mass -= flow;

                // Update the water contents in the cell below
                new_board->grid[index_below].mass += flow;
            }

            // Step 4) Move water up
            if (y != NY - 1) {
                int index_above = INDEX_OF_POS(x, y + 1);
                Cell cell_above = board->grid[index_above];
                if (cell_above.state != states_obstacle) {
                    float mass_to_move =
                        cur_mass -
                        get_stable_state_b(cur_mass + cell_above.mass);
                    if (mass_to_move > 0) {
                        float flow = MAX(cur_mass, mass_to_move);
                        flow = BOARD_LIMIT_FLOW(flow);
                        cur_mass -= flow;
                        // Update the water contents in the cell above
                        new_board->grid[index_above].mass += flow;
                    }
                }
            }

            // This isn't in the source but surely it is like this. Otherwhise
            // all the water but the flow is gone from this cell
            new_board->grid[cur_index].mass += cur_mass;
        }
    }

    // Calculate the states of the cells
    for (int x = 0; x < NX; x++) {
        for (int y = 0; y < NY; y++) {
            int index = INDEX_OF_POS(x, y);
            if (board->grid[index].state == states_obstacle) {
                new_board->grid[index].state = states_obstacle;
                board_inc_counter(new_board, states_obstacle);
            } else if (board->grid[index].mass >= BOARD_MIN_MASS) {
                new_board->grid[index].state = states_water;
                board_inc_counter(new_board, states_water);
            }
        }
    }

    // Copy new board to board
    memcpy(board, new_board, sizeof(*board));
}