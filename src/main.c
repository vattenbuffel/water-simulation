#include "../include/glad/glad.h"
#include "circle_creator.h"
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
 *   Implementera hinder
 *   Kunna måla vatten respektive hinder
 *
 */

#define NX 10
#define NY 10

#define SCR_WIDTH 800
#define SCR_HEIGHT 800

// Radius of the water circles
#define RADIUS (SCR_WIDTH / NX)

// Macro to calculate the index of x of position position x,y in the grid array
#define INDEX_OF_POS_X(x, y) (y * NX * 3 + x * 3 + 0)
// Macro to calculate the index of y of position position x,y in the grid array
#define INDEX_OF_POS_Y(x, y) (y * NX * 3 + x * 3 + 1)
// Macro to calculate the index of state of position position x,y in the grid
// array
#define INDEX_OF_POS_STATE(x, y) (y * NX * 3 + x * 3 + 2)

// Macro to calculate the x pixel value of pos x in grid
#define POS_TO_PIXEL(x) (x / RADIUS)
// Macro to calculate the y pixel value of pos y in grid
#define POS_TO_PIXEL(y) (y / RADIUS)


enum States {
    states_background,
    states_water,
    // states_obstacle,
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

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.0f, 0.5f, 0.8f, 1.0f);\n"
    "}\n\0";

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
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s\n", infoLog);
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING::FAILED: %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create the grid of the water dropplet positions
    int n_circles = 0;
    static float new_grid[NY * NX * 3];
    // static float old_grid[NY * NX * 3];
    for (int y = 0; y < NY; y++) {
        for (int x = 0; x < NX; x++) {
            new_grid[INDEX_OF_POS_X(x, y)] = x;
            new_grid[INDEX_OF_POS_Y(x, y)] = y;

            float state = (float)(rand() % states_max_val);
            assert(state < states_max_val && state >= 0);
            new_grid[INDEX_OF_POS_STATE(x, y)] = states_water; // state;
            n_circles += 1; // state==states_water;
        }
    }

    // Create the indices array for the circles
    unsigned int indices[CIRCLES_INDEX_ARRAY_LENGTH(n_circles)];
    connecting_vertices(indices, n_circles);

    // Create the arrays containing the pixel locations of the circle
    float triangle_circles[CIRCLES_VERTICES_ARRAY_LENGTH(n_circles)];
    float x_centers[] = {100, 700, 700, 100};
    float y_centers[] = {100, 100, 500, 500};
    init_triangle_circles(triangle_circles, RADIUS, x_centers, y_centers,
                          n_circles);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s),
    // and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_circles), triangle_circles,
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // Positions attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // // State attribute
    // glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
    //                       (void *)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object so afterwards we
    // can safely unbind
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally
    // modify this VAO, but this rarely happens. Modifying other VAOs requires a
    // call to glBindVertexArray aNYways so we generally don't unbind VAOs (nor
    // VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

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

        // input
        // -----
        processInput(window);

        // render
        // ------
        // glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glPointSize(10);
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's
                                // no need to bind it every time, but we'll do
                                // so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, CIRCLES_INDEX_ARRAY_LENGTH(n_circles),
                       GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

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
