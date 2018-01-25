#include "physics.h"
#include "math_extra.h"
// Include header files for platform
#include "mbed.h"

// Include header files for project
#include "globals.h"
#include "game.h"
#include "wall.h"
#include "doublely_linked_list.h"

Serial pc(USBTX,USBRX);     // used by Accelerometer
MMA8452 acc(p28, p27, 100000); // Accelerometer

void forward_euler(Physics* state, float delta, int* coeff)
{ 
    // Update position and velocity
    float mod = ((float) *coeff)/10.0;
    state->px = clamp(state->px + mod*(delta*state->vx),124);
    if (state->px < 4) {
        state->px = 4;
    }
    state->py = clamp(state->py + mod*(delta*state->vy),124);
    if (state->py < 4) {
        state->py = 4;
    }
    state->vx = .9995*(state->vx - (250*delta)*state->ax);
    state->vy = .9995*(state->vy + (250*delta)*state->ay);
}