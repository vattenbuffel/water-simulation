#include "keyboard.h"
#include <stdbool.h>
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "board.h"

//======================================//
// Local function declarations
//======================================//

//======================================//
// Local function implementations
//======================================//


//======================================//
// Global function implementations
//======================================//

void keyboard_init(Keyboard* keyboard){
    keyboard->restart = false;
    keyboard->state = states_background;
}