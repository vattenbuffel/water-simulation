#include "../include/glad/glad.h"
#include "assert_.h"
#include "board.h"
#include "circle_creator.h"
#include "constants.h"
#include "keyboard.h"
#include "obstacle_creator.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* TODO:
 *   Implementera automata för simulation av vattnets rörelser
 *   Massively speed up calculating the positions of water
 *   The water should be darker the more there is
 */

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

/**
 * @brief Changes the mouse state to be either GLFW_PRESS or GLFW_RELEASE
 *
 * @param window
 * @param button
 * @param action
 * @param mods
 */
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);

/**
 * @brief Whenever the mouse is moved. Used for placing or removing obstacles
 *
 * @param window
 * @param xpos
 * @param ypos
 */
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

void processInput(GLFWwindow *window);
void modify_grid(int x, int y, int resulting_state);
void fall(float *all_pixels, float *updated_pixels);

// state of mouse. either pressed or not pressed
int mouse_state = GLFW_RELEASE;
// State of keyboard.
Keyboard keyboard;

// Board state
Board board;
Board new_board;

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "float x = (aPos.x-400)/400;\n"
                                 "float y = (aPos.y-400)/400;\n"
                                 "   gl_Position = vec4(x, y, aPos.z, 1.0);\n"
                                 "}\0";

const char *fragmentShaderSourceCircles =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.0f, 0.5f, 0.8f, 0.3f);\n"
    "}\n\0";

const char *fragmentShaderSourceObstacles =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.0f, 0.0f, 0.0f, 0.3f);\n"
    "}\n\0";

int main() {
    // Init the board
    board_init(&board);
    board_init(&new_board);
    // Init keyboard
    keyboard_init(&keyboard);

    srand(time(NULL)); // Initialization, should only be called once.
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

    // glfw window creation
    // --------------------
    GLFWwindow *window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window!\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glfwSwapInterval(0); // SUUUUUPER FAST
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to init glad!\n");
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", infoLog);
    }
    // fragment shader circle
    unsigned int fragmentShaderCircles = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderCircles, 1, &fragmentShaderSourceCircles,
                   NULL);
    glCompileShader(fragmentShaderCircles);
    // check for shader compile errors
    glGetShaderiv(fragmentShaderCircles, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderCircles, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s\n", infoLog);
    }
    // link shaders circle
    unsigned int shaderProgramCircles = glCreateProgram();
    glAttachShader(shaderProgramCircles, vertexShader);
    glAttachShader(shaderProgramCircles, fragmentShaderCircles);
    glLinkProgram(shaderProgramCircles);
    // check for linking errors
    glGetProgramiv(shaderProgramCircles, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramCircles, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING::FAILED: %s\n", infoLog);
    }
    glDeleteShader(fragmentShaderCircles);

    // fragment shader obstacles
    unsigned int fragmentShaderObstacles = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObstacles, 1, &fragmentShaderSourceObstacles,
                   NULL);
    glCompileShader(fragmentShaderObstacles);
    // check for shader compile errors
    glGetShaderiv(fragmentShaderObstacles, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShaderObstacles, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s\n", infoLog);
    }
    // link shaders circle
    unsigned int shaderProgramObstacles = glCreateProgram();
    glAttachShader(shaderProgramObstacles, vertexShader);
    glAttachShader(shaderProgramObstacles, fragmentShaderObstacles);
    glLinkProgram(shaderProgramObstacles);
    // check for linking errors
    glGetProgramiv(shaderProgramObstacles, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramObstacles, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING::FAILED: %s\n", infoLog);
    }
    glDeleteShader(fragmentShaderObstacles);
    glDeleteShader(vertexShader);

    // Create the grid of the water dropplet and obstacle positions
    // for (int y = 0; y < NY; y++) {
    //     for (int x = 0; x < NX; x++) {
    //         float state = (float)(rand() % states_max_val);
    //         assert_(state < states_max_val && state >= 0,
    //                 "State value must be between states_max_cal and 0.");
    //         state = states_water; // TEMP
    //         board.grid[INDEX_OF_POS(x, y)] = state;
    //         board_inc_counter(&board, state);
    //     }
    // }

    // Create the indices array for the circles
    unsigned int circles_indices[CIRCLES_VERTICES_ARRAY_LENGTH(NX * NY)];
    circles_connecting_vertices(circles_indices, NX * NY);

    // Create the arrays containing the pixel locations of the circles
    static float circles_vertices[CIRCLES_VERTICES_ARRAY_LENGTH(NX * NY)];

    // Create the indices array for the obstacles
    unsigned int obstacles_indices[OBSTACLES_INDEX_ARRAY_LENGTH(NX * NY)];
    obstacles_connecting_vertices(obstacles_indices, NX * NY);

    // Create the arrays containing the pixel locations of the obstacles
    static float obstacles_vertices[OBSTACLES_VERTICES_ARRAY_LENGTH(NX * NY)];

    unsigned int circles_VBO, circles_VAO, circles_EBO, obstacles_VBO,
        obstacles_VAO, obstacles_EBO;
    glGenVertexArrays(1, &circles_VAO);
    glGenBuffers(1, &circles_VBO);
    glGenBuffers(1, &circles_EBO);
    glGenVertexArrays(1, &obstacles_VAO);
    glGenBuffers(1, &obstacles_VBO);
    glGenBuffers(1, &obstacles_EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(circles_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, circles_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circles_vertices), circles_vertices,
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circles_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circles_indices),
                 circles_indices, GL_STATIC_DRAW);

    // Positions attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(obstacles_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, obstacles_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(obstacles_vertices),
                 obstacles_vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obstacles_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(obstacles_indices),
                 obstacles_indices, GL_STATIC_DRAW);

    // Positions attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // // State attribute
    // glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
    //                       (void *)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    double t_prev = glfwGetTime();
    int print_every = 100;
    int i = 0;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        i++;
        if (i == print_every) {
            double t_now = glfwGetTime();
            float fps = print_every / (t_now - t_prev);
            printf("FPS: %f\n", fps);
            t_prev = t_now;
            i = 0;
        }

        // Simulation logic
        if (keyboard.run_simulation)
            board_simulate(&board, &new_board);

        // Calculate the water droplets and obstacles based on grid
        board_circle_from_grid(board.grid, circles_vertices, board.n_circles);
        board_obstacles_from_grid(board.grid, obstacles_vertices,
                                  board.n_obstacles);

        // input
        // -----
        processInput(window);

        // render
        // ------
        // glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glPointSize(10);

        // Draw circles
        glUseProgram(shaderProgramCircles);
        glBindVertexArray(circles_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, circles_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circles_vertices),
                     circles_vertices, GL_DYNAMIC_DRAW);
        int bajs = CIRCLES_INDEX_ARRAY_LENGTH(board.n_circles);
        glDrawElements(GL_TRIANGLES,
                       CIRCLES_INDEX_ARRAY_LENGTH(board.n_circles),
                       GL_UNSIGNED_INT, 0);

        // Draw obstacles
        glUseProgram(shaderProgramObstacles);
        glBindVertexArray(obstacles_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, obstacles_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(obstacles_vertices),
                     obstacles_vertices, GL_DYNAMIC_DRAW);
        glDrawElements(GL_TRIANGLES,
                       OBSTACLES_INDEX_ARRAY_LENGTH(board.n_obstacles),
                       GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        int tehaiontoieahn = 5 + 5;
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &circles_VAO);
    glDeleteBuffers(1, &circles_VBO);
    glDeleteProgram(shaderProgramCircles);
    glDeleteProgram(shaderProgramObstacles);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    bool should_restart = false; // Restarts should be abolished if anything
                                 // but ENTER is pressed after R

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_1)) {
        // printf("Switching to keyboard state water\n");
        keyboard.state = states_water;
    } else if (glfwGetKey(window, GLFW_KEY_2)) {
        // printf("Switching to keyboard state obstacle\n");
        keyboard.state = states_obstacle;
    } else if (glfwGetKey(window, GLFW_KEY_3)) {
        // printf("Switching to keyboard state background\n");
        keyboard.state = states_background;
    } else if (glfwGetKey(window, GLFW_KEY_R)) {
        printf("Restart initialized.\nPress ENTER to restart board.\n");
        should_restart = true;
    } else if (glfwGetKey(window, GLFW_KEY_ENTER)) {
        if (keyboard.restart) {
            printf("Going to restart board\n");
            keyboard.run_simulation = false;
            board_restart(&board);
        }
    } else if (glfwGetKey(window, GLFW_KEY_A)) {
        printf("Simulation should start\n");
        keyboard.run_simulation = true;
    } else if (glfwGetKey(window, GLFW_KEY_D)) {
        printf("Simulation should stop\n");
        keyboard.run_simulation = false;
    } else {
        // printf("Unhandled keystroke!\n");
        should_restart = keyboard.restart;
    }
    keyboard.restart = should_restart;
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    // printf("Mouse at x: %f and y: %f\n", xpos, ypos);
    // printf("Mouse at grid index: %d\n", GLFW_POS_TO_GRID_INDEX(xpos, ypos));

    // For some reason it seems that xpos and ypos can be outside of the window
    if (xpos > SCR_WIDTH || xpos < 0)
        return;
    else if (ypos > SCR_HEIGHT || ypos < 0)
        return;

    // int grid_x = PIXEL_X_TO_GRID_X(xpos - 1);
    // int grid_y = PIXEL_Y_TO_GRID_Y(SCR_HEIGHT - ypos - 1);
    // printf("pixel x to grid x: %d, pixel y to grid y: %d, grid index: %d\n",
    //        grid_x, grid_y, INDEX_OF_POS(grid_x, grid_y));
    // assert_(grid_x <= NX - 1 && grid_x >= 0,
    //         "Grid_x must be smaller than NX and greater than 0");
    // assert_(grid_y <= NY - 1 && grid_y >= 0,
    //         "Grid_y must be smaller than NY and greater than 0");
    // printf("\n");

    // If the mouse is pressed then an object should be either drawn or removed
    if (mouse_state == GLFW_PRESS) {
        // What to draw is basd on keyboard state
        int grid_x = PIXEL_X_TO_GRID_X(xpos - 1);
        int grid_y = PIXEL_Y_TO_GRID_Y(SCR_HEIGHT - ypos - 1);
        board_modify_grid(&board, grid_x, grid_y, keyboard.state);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
    mouse_state = action;

    // This code prints what key is pressed or released
    // char* button_action = action == GLFW_RELEASE ? "released" : "pressed";
    // if(button == GLFW_MOUSE_BUTTON_LEFT)
    //     printf("Left mouse button was %s!\n", button_action);
    // else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    //     printf("Left mouse button was %s!\n", button_action);
    // else
    //     printf("Unhandled button with id %d was %s\n", action,
    //     button_action);
}

// void fall(float *old_grid, float *new_grid) {
//     float state;
//     for (int y = 1; y < NY; y++) {
//         for (int x = 0; x < NX; x++) {
//             state = old_grid[y * NX * 3 + x * 3 + 2];
//             if (state !=
//                 states_max_val) // This should really be static/obstacles
//                 state new_grid[(y - 1) * NX * 3 + x * 3 + 2] = state;
//         }
//     }

//     for (int x = 0; x < NX; x++) {
//         state = new_grid[(NY - 1) * NX * 3 + x * 3 + 2];
//         if (state == states_water)
//             new_grid[(NY - 1) * NX * 3 + x * 3 + 2] = states_background;
//     }
// }
