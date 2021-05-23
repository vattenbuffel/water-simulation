#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "board.h"

//======================================//
// Defines
//======================================//



typedef struct Keyboard Keyboard;

struct Keyboard{
    // Keeps track of what key was latest pressed. Can be either of state enum values.
    state_type state;
    // Keeps track of if a restart is initiated
    bool restart;
    // If we should simulate or not
    bool run_simulation;
};

//======================================//
// Global function declarations
//======================================//
void keyboard_init(Keyboard* keyboard);


#endif //KEYBOARD_H