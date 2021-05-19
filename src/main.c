#include "../include/glad/glad.h"
#include "circle_creator.h"
#include "obstacle_creator.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* TODO:
 *   Implementera automata för simulation av vattnets rörelser
 *   Måla in vatten och hinder med musen
 *   Kunna måla vatten respektive hinder
 *   Massively speed up calculating the positions of water
 *
 */

#define NX 100
#define NY 100

#define SCR_WIDTH 800
#define SCR_HEIGHT 800

// Radius of the water circles
// #define RADIUS (2 * SCR_WIDTH / NX)
#define RADIUS (1 * SCR_WIDTH / NX)

// Side length of obstacle
#define OBSTACLE_SIDE_LENGTH (2 * RADIUS)

// Macro to calculate the index of state of position position x,y in the grid
// array
#define INDEX_OF_POS(x, y) (y * NX + x)

// Macro to go from index in grid to col
#define INDEX_TO_COL(i) (i % NX)

// Macro to go from index in grid to row
#define INDEX_TO_ROW(i) ((int)i / NX)

// Macro to calculate the x pixel value of col x in grid
#define COL_TO_PIXEL_X(x) (x * (float)SCR_WIDTH / NX)
// Macro to calculate the y pixel value of row y in grid
#define ROW_TO_PIXEL_Y(y) (y * (float)SCR_HEIGHT / NY)

// Macro to go from index in grid to x pixel
#define INDEX_TO_X(i) (COL_TO_PIXEL_X(INDEX_TO_COL(i)))

// Macro to go from index in grid to y pixel
#define INDEX_TO_Y(i) (ROW_TO_PIXEL_Y(INDEX_TO_ROW(i)))

enum States {
    states_background,
    states_water,
    states_obstacle,
    states_max_val
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void fall(float *all_pixels, float *updated_pixels);

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

void circle_from_grid(int *grid, float *vertices, int n_circles) {
    int center_i = 0;
    float center_x[n_circles];
    float center_y[n_circles];
    for (int i = 0; i < NX * NY; i++) {
        int state = grid[i];
        if (state == states_water) {
            center_x[center_i] = INDEX_TO_X(i);
            center_y[center_i] = INDEX_TO_Y(i);
            center_i++;
        }
    }

    init_triangle_circles(vertices, RADIUS, center_x, center_y, n_circles);
}

void obstacles_from_grid(int *grid, float *vertices, int n_obstacles) {
    int center_i = 0;
    float center_x[n_obstacles];
    float center_y[n_obstacles];
    for (int i = 0; i < NX * NY; i++) {
        int state = grid[i];
        if (state == states_obstacle) {
            center_x[center_i] = INDEX_TO_X(i);
            center_y[center_i] = INDEX_TO_Y(i);
            center_i++;
        }
    }

    obstacles_init(vertices, OBSTACLE_SIDE_LENGTH, center_x, center_y,
                   n_obstacles);
}

int main() {
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
    glShaderSource(fragmentShaderCircles, 1, &fragmentShaderSourceCircles, NULL);
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
    glShaderSource(fragmentShaderObstacles, 1, &fragmentShaderSourceObstacles, NULL);
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
    int n_circles, n_obstacles = 0;
    static int new_grid[NY * NX] = {0};
    // static float old_grid[NY * NX * 3];
    for (int y = 0; y < NY; y++) {
        for (int x = 0; x < NX; x++) {
            float state = (float)(rand() % states_max_val);
            assert(state < states_max_val && state >= 0);
            new_grid[INDEX_OF_POS(x, y)] = state;
            n_circles += state == states_water;
            n_obstacles += state == states_obstacle;
        }
    }

    // Create the indices array for the circles
    // static unsigned int indices[NX*NY];
    unsigned int circles_indices[CIRCLES_VERTICES_ARRAY_LENGTH(n_circles)];
    circles_connecting_vertices(circles_indices, n_circles);

    // Create the arrays containing the pixel locations of the circles
    static float circles_vertices[CIRCLES_VERTICES_ARRAY_LENGTH(NX * NY)];

    // Create the indices array for the obstacles
    // static unsigned int indices[NX*NY];
    unsigned int
        obstacles_indices[OBSTACLES_INDEX_ARRAY_LENGTH(n_obstacles)];
    obstacles_connecting_vertices(obstacles_indices, n_obstacles);

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

    // // Positions attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // // State attribute
    // glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
    //                       (void *)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered
    // circles_VBO as the vertex attribute's bound vertex buffer object so
    // afterwards we can safely unbind glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the circles_VAO afterwards so other circles_VAO calls
    // won't accidentally modify this circles_VAO, but this rarely happens.
    // Modifying other circles_VAOs requires a call to glBindVertexArray aNYways
    // so we generally don't unbind circles_VAOs (nor circles_VBOs) when it's
    // not directly necessary. glBindVertexArray(0);

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
        // memcpy(old_grid, new_grid, sizeof(new_grid));
        // fall(old_grid, new_grid);

        // Calculate the water droplets and obstacles based on grid
        circle_from_grid(new_grid, circles_vertices, n_circles);
        obstacles_from_grid(new_grid, obstacles_vertices, n_obstacles);

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
        glDrawElements(GL_TRIANGLES, CIRCLES_INDEX_ARRAY_LENGTH(n_circles),
                       GL_UNSIGNED_INT, 0);

        // Draw obstacles
        glUseProgram(shaderProgramObstacles);
        glBindVertexArray(obstacles_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, obstacles_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(obstacles_vertices),
                     obstacles_vertices, GL_DYNAMIC_DRAW);
        glDrawElements(GL_TRIANGLES, OBSTACLES_INDEX_ARRAY_LENGTH(n_obstacles),
                       GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

void fall(float *old_grid, float *new_grid) {
    float state;
    for (int y = 1; y < NY; y++) {
        for (int x = 0; x < NX; x++) {
            state = old_grid[y * NX * 3 + x * 3 + 2];
            if (state !=
                states_max_val) // This should really be static/obstacles state
                new_grid[(y - 1) * NX * 3 + x * 3 + 2] = state;
        }
    }

    for (int x = 0; x < NX; x++) {
        state = new_grid[(NY - 1) * NX * 3 + x * 3 + 2];
        if (state == states_water)
            new_grid[(NY - 1) * NX * 3 + x * 3 + 2] = states_background;
    }
}
