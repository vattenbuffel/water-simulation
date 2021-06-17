#include "board.h"
#include "assert_.h"
#include "circle_creator.h"
#include "obstacle_creator.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
            float cur_mass = cur_cell.mass;

            // Only water cells have any simulation
            if (cur_cell.state != states_water)
                continue;

            // Step 1) Move water down
            int index_below = INDEX_OF_POS(x, y - 1);
            Cell cell_below = board->grid[index_below];

            // Only do something if cell below is not obstacle and this cell
            // isn't is at the bottom
            if (cell_below.state != states_obstacle && y != 0) {
                float mass_below = cell_below.mass;
                float possible_mass_below =
                    get_stable_state_b(cur_mass + mass_below);
                float mass_to_move = possible_mass_below - mass_below;
                float flow = MIN(cur_mass, mass_to_move);
                flow = BOARD_LIMIT_FLOW(flow);
                if (flow<0) printf("flow: %f\n", flow);
                cur_mass -= flow;

                // Update the water contents in the cell below
                new_board->grid[index_below].mass += flow;
            }

            // Step 2) Move water left
            if (x != 0) {
                int index_left = INDEX_OF_POS(x - 1, y);
                Cell cell_left = board->grid[index_left];
                if (cell_left.state != states_obstacle) {
                    float mass_to_move =
                        (cur_mass - cell_left.mass)/BOARD_SIDEWAYS_DIV_FACTOR;
                    if (mass_to_move >= BOARD_MIN_FLOW) {
                        float flow = BOARD_LIMIT_FLOW(mass_to_move);
                        cur_mass -= flow;
                        // Update the water contents in the cell to the left
                        new_board->grid[index_left].mass += flow;
                    }
                }
            }

            // Step 3) Move water right
            if (x != NX - 1) {
                int index_right = INDEX_OF_POS(x + 1, y);
                Cell cell_right = board->grid[index_right];
                if (cell_right.state != states_obstacle) {
                    float mass_to_move =
                        (cur_mass - cell_right.mass)/(BOARD_SIDEWAYS_DIV_FACTOR-1.0f);
                    if (mass_to_move >= BOARD_MIN_FLOW) {
                        float flow = BOARD_LIMIT_FLOW(mass_to_move);
                        cur_mass -= flow;
                        // Update the water contents in the cell to the right
                        new_board->grid[index_right].mass += flow;
                    }
                }
            }

            // Step 4) Move water up
            if (y != NY - 1) {
                int index_above = INDEX_OF_POS(x, y + 1);
                Cell cell_above = board->grid[index_above];
                if (cell_above.state != states_obstacle) {
                    float mass_to_move =
                        cur_mass -
                        get_stable_state_b(cur_mass + cell_above.mass);
                    if (mass_to_move > BOARD_MIN_FLOW) {
                        float flow = MIN(cur_mass, mass_to_move);
                        flow = BOARD_LIMIT_FLOW(flow);
                        cur_mass -= flow;
                        // Update the water contents in the cell above
                        new_board->grid[index_above].mass += flow;
                    }
                }
            }

            new_board->grid[cur_index].mass += cur_mass;
            char aethiopaethbo = 5 + 5;
        }
    }

    // Calculate the states of the cells
    double total_mass_in_system = 0;
    for (int x = 0; x < NX; x++) {
        for (int y = 0; y < NY; y++) {
            int index = INDEX_OF_POS(x, y);
            if (board->grid[index].state == states_obstacle) {
                new_board->grid[index].state = states_obstacle;
                board_inc_counter(new_board, states_obstacle);
            } else if (new_board->grid[index].mass >= BOARD_MIN_MASS) {
                new_board->grid[index].state = states_water;
                board_inc_counter(new_board, states_water);
            }
            else if(new_board->grid[index].mass < BOARD_MIN_MASS){
                new_board->grid[index].state = states_background;
                new_board->grid[index].mass = 0;
            }
            else{
                printf("Something went wrong in board simulation\n");
                assert_(false, "Something went wrong in board simulation\n");
            }
            total_mass_in_system += new_board->grid[index].mass;
        }
    }
    printf("total_mass_in_system %f\n", total_mass_in_system);

    // Copy new board to board
    memcpy(board, new_board, sizeof(*board));
}