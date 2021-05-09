#include "../src/circle_creator.h"
#include "../src/obstacle_creator.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NX 800
#define NY 800


void test_triangle_cirlce() {
    // See if the macros output the correct lengths
    assert(CIRCLE_VERTICES_ARRAY_LENGTH == N_TRIANGLES * 3 + 3);
    assert(CIRCLES_VERTICES_ARRAY_LENGTH(1) == N_TRIANGLES * 3 + 3);
    assert(CIRCLES_VERTICES_ARRAY_LENGTH(2) == 2 * (N_TRIANGLES * 3 + 3));
    assert(CIRCLE_INDEX_ARRAY_LENGTH == N_TRIANGLES * 3);
    assert(CIRCLES_INDEX_ARRAY_LENGTH(1) == N_TRIANGLES * 3);
    assert(CIRCLES_INDEX_ARRAY_LENGTH(2) == 2 * N_TRIANGLES * 3);

    // See if it creates the correct center
    float vertices[CIRCLE_VERTICES_ARRAY_LENGTH] = {0};
    init_triangle_circle(vertices, 1, 1, 2);
    assert(vertices[0] == 1);
    assert(vertices[1] == 2);
    assert(vertices[2] == 0);

    // See if it leaves the last element of the array untouched
    float vertices_[CIRCLE_VERTICES_ARRAY_LENGTH + 1] = {0};
    init_triangle_circle(vertices_, 1, 1, 2);
    assert(vertices[CIRCLE_VERTICES_ARRAY_LENGTH] == 0);

    // See if the second x and y position are correct
    init_triangle_circle(vertices, 5, 1, 2);
    assert(abs(vertices[3 * 3 + 0] - 5 * cos(2 * M_PI / N_TRIANGLES * 2) - 1) <
           0.0001);
    assert(abs(vertices[3 * 3 + 1] - 5 * sin(2 * M_PI / N_TRIANGLES * 2) - 2) <
           0.0001);

    // See if the second x and y positions of the second circle is correct
    float center_x[2] = {0, 1};
    float center_y[2] = {0, -2};
    float vertices__[CIRCLES_VERTICES_ARRAY_LENGTH(2)];
    init_triangle_circles(vertices__, 3, center_x, center_y, 2);
    assert(abs(vertices__[3 * 3 + 0 + CIRCLE_VERTICES_ARRAY_LENGTH] -
               3 * cos(2 * M_PI / N_TRIANGLES * 2) - 1) < 0.0001);
    assert(abs(vertices__[3 * 3 + 1 + CIRCLE_VERTICES_ARRAY_LENGTH] -
               3 * sin(2 * M_PI / N_TRIANGLES * 2) + 2) < 0.0001);

    // See if the indexes created are correct
    int indexes[CIRCLES_INDEX_ARRAY_LENGTH(2)];
    circles_connecting_vertices(indexes, 2);
    assert(indexes[0] == 0);
    assert(indexes[1] == 1);
    assert(indexes[2] == 2);
    assert(indexes[3] == 0);
    assert(indexes[4] == 2);
    assert(indexes[5] == 3);
    assert(indexes[CIRCLE_INDEX_ARRAY_LENGTH - 1] == 1);

    assert(indexes[CIRCLE_INDEX_ARRAY_LENGTH + 0] == N_TRIANGLES + 0);
    assert(indexes[CIRCLE_INDEX_ARRAY_LENGTH + 1] == N_TRIANGLES + 1);
    assert(indexes[CIRCLE_INDEX_ARRAY_LENGTH + 2] == N_TRIANGLES + 2);
    assert(indexes[CIRCLE_INDEX_ARRAY_LENGTH + 3] == N_TRIANGLES + 0);
    assert(indexes[CIRCLE_INDEX_ARRAY_LENGTH + 4] == N_TRIANGLES + 2);
    assert(indexes[CIRCLE_INDEX_ARRAY_LENGTH + 5] == N_TRIANGLES + 3);
}

void test_obstacle() {
// See if the macros output the correct lengths
    assert(OBSTACLE_VERTICES_ARRAY_LENGTH == 12);
    assert(OBSTACLES_VERTICES_ARRAY_LENGTH(1) == 12);
    assert(OBSTACLES_VERTICES_ARRAY_LENGTH(2) == 2 * 12);
    assert(OBSTACLE_INDEX_ARRAY_LENGTH == 6);
    assert(OBSTACLES_INDEX_ARRAY_LENGTH(1) == 6);
    assert(OBSTACLES_INDEX_ARRAY_LENGTH(2) == 2 * 6);

   
    // See if the second x and y position are correct
    float vertices[OBSTACLES_VERTICES_ARRAY_LENGTH(2)];
    obstacle_init(vertices, 5, 1, 2);
    assert(abs(vertices[3 + 0] - 5/sqrt(2) * cos(DEG_TO_RAD(135)) - 1) <
           0.0001);
    assert(abs(vertices[3 + 1] - 5/sqrt(2) * sin(DEG_TO_RAD(135)) - 2) <
           0.0001);

    // See if the second x and y positions of the second obstacle are correct
    float center_x[2] = {0, 1};
    float center_y[2] = {0, -2};
    obstacles_init(vertices, 3, center_x, center_y, 2);
    int idx = 3 + 0 + OBSTACLE_VERTICES_ARRAY_LENGTH;
    assert(abs(vertices[3 + 0] - 3/sqrt(2) * cos(DEG_TO_RAD(135)) - 0) <
           0.0001);
    assert(abs(vertices[3 + 1] - 3/sqrt(2) * sin(DEG_TO_RAD(135)) + 0) <
           0.0001);
    assert(abs(vertices[3 + 0 + OBSTACLE_VERTICES_ARRAY_LENGTH] -
               3/sqrt(2) * cos(DEG_TO_RAD(135)) - 1) < 0.0001);
    assert(abs(vertices[3 + 1 + OBSTACLE_VERTICES_ARRAY_LENGTH] -
               3/sqrt(2) * sin(DEG_TO_RAD(135)) + 2) < 0.0001);

    // See if the indexes created are correct
    int indexes[OBSTACLES_INDEX_ARRAY_LENGTH(2)];
    obstacle_connecting_vertices(indexes, 2);
    assert(indexes[0] == 0);
    assert(indexes[1] == 2);
    assert(indexes[2] == 1);
    assert(indexes[3] == 0);
    assert(indexes[4] == 2);
    assert(indexes[5] == 3);
    
    assert(indexes[OBSTACLE_INDEX_ARRAY_LENGTH + 0] == 4 + 0);
    assert(indexes[OBSTACLE_INDEX_ARRAY_LENGTH + 1] == 4 + 2);
    assert(indexes[OBSTACLE_INDEX_ARRAY_LENGTH + 2] == 4 + 1);
    assert(indexes[OBSTACLE_INDEX_ARRAY_LENGTH + 3] == 4 + 0);
    assert(indexes[OBSTACLE_INDEX_ARRAY_LENGTH + 4] == 4 + 2);
    assert(indexes[OBSTACLE_INDEX_ARRAY_LENGTH + 5] == 4 + 3);


}

int main(int argc) {
    // test_triangle_cirlce(); // TODO: these tests need updating
    test_obstacle();
    printf("Passed all tests!\n");
    return 0;
}