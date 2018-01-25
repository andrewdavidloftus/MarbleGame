#pragma once

#include "physics.h"
#include "doublely_linked_list.h"

/* A structure for holding all the game inputs */
struct GameInputs
{
    // Measurements from the accelerometer
    double ax, ay, az;
    // Button Measurements;
    int b1, b2, b3, b4;
};

/* A structure for holding saved states */
struct GameSave
{
    // Values for position and velocity
    int px, py, vx, vy;
};

//////////////////////////
// Arena Element structs
//////////////////////////
/** The basic ArenaElement structure.
    Every item in an arena DLinkedList should be able to be cast to this struct.
    The type member is used to tell what other struct this element might be.
*/
struct ArenaElement {
    int type;
};

// Element types
#define WALL        0
#define BALL        1
#define POTHOLE     2
#define MUD         3
#define TELE        4
#define COIN        5
#define FINISH      6

/** An ArenaElement struct representing the ball. */
struct Ball {
    // ArenaElement type (must be first element)
    int type;
    // Drawing info
    int x, y;
};

/** An AreaElement struct representing the pothole. */
struct Pothole {
    // ArenaElement type
    int type;
    // Drawing Info
    int x, y, r;
};

/** An AreaElement struct representing the mud. */
struct Mud {
    // ArenaElement type
    int type;
    // Drawing Info
    int x1, y1, x2, y2;
};

/** An AreaElement struct representing the portal. */
struct Tele {
    // ArenaElement type
    int type;
    // Drawing Info
    int x1, y1, x2, y2;
};

/** An AreaElement struct representing the TEMP. */
struct Coin {
    // ArenaElement type
    int type;
    // Drawing Info
    int x, y;
    // Eaten yet? Don't draw if already passed over/eaten.
    int draw;
};

/** An AreaElement struct representing the finish. */
struct Finish {
    // ArenaElement type
    int type;
    // Drawing Info
    int x, y;
};

/////////////////////////
// ArenaElement helpers
/////////////////////////
/** Erases the ball */
void erase_ball(Ball* ball);
/** Draws the ball at the current state */
void draw_ball(Ball* ball, Physics* state);

/* Add additional helpers for other ArenaElement types here */

///////////////////////////
// Game control functions 
///////////////////////////
/* Reads all game inputs */
GameInputs read_inputs();

/* Performs a single physics update. */
int update_game(DLinkedList* arena, Physics* prev, GameInputs inputs, float delta);

/* Implements the game loop */
int run_game(DLinkedList* arena, Physics* state);
