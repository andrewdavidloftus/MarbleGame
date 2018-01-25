#include "game.h"

#include "globals.h"
#include "physics.h"
#include "wall.h"
#include "math_extra.h"


/** Erases the ball from the screen by drawing over it with the background color. */
void erase_ball(Ball* ball)
{
    int bx, by, r;
    r = 4;
    bx = ball->x;
    by = ball->y;
    uLCD.filled_circle(bx , by , r, DGREY);
}

/** Draws the ball on the screen at the updated location (according to the state) */
void draw_ball(Ball* ball, Physics* state)
{
    int bx, by, r;
    r = 4;
    bx = state->px;
    by = state->py;
    ball->x = bx;
    ball->y = by;
    uLCD.filled_circle(bx , by , r, WHITE);
}

void draw_finish(Finish* finish) {
    int x, y, r;
    r = 6;
    x = finish->x;
    y = finish->y;
    uLCD.circle(x, y, r, GREEN);
    uLCD.circle(x, y, r-1, GREEN);
}

int do_finish(Physics* next, Finish* finish) {
    int distX = next->px - finish->x;
    int distY = next->py - finish->y;
    distX = distX*distX;
    distY = distY*distY;
    int dist;
    dist = distX + distY;
    if (dist < 5) {
        return 1;
    }
    return 0;
}

void draw_pothole(Pothole* ph) {
    int x, y, r;
    r = ph->r;
    x = ph->x;
    y = ph->y;
    uLCD.filled_circle(x, y, r, BLACK);
}

void do_pothole(Physics* next, Physics* curr, Pothole* ph) {
    int distX = curr->px - ph->x;
    int distY = curr->py - ph->y;
    distX = distX*distX;
    distY = distY*distY;
    int dist;
    dist = distX + distY;
    int r = ph->r;
    if (dist < (r + 4)) {
        next->px = 64;
        next->py = 64;
        next->vx = 0;
        next->vy = 0;
    }
}

void draw_coin(Coin* coin) {
    if (coin->draw == 1) {
        int x, y, r;
        r = 3;
        x = coin->x;
        y = coin->y;
        uLCD.filled_circle(x, y, r, GOLD);
    }
}

void do_coin(int* coinCount, Physics* curr, Coin* coin) {
    int distX = curr->px - coin->x;
    int distY = curr->py - coin->y;
    distX = distX*distX;
    distY = distY*distY;
    int dist;
    dist = distX + distY;
    if (dist < 5) {
        coin->draw = 0;
        *coinCount = *coinCount + 20;
        uLCD.filled_circle(coin->x, coin->y, 3, DGREY);
    }
}

void draw_tele(Tele* tele) {
    uLCD.filled_circle(tele->x1, tele->y1, 5, BBLUE);
    uLCD.filled_circle(tele->x2, tele->y2, 5, ORANGE);
}

void do_tele(Physics* next, Physics* curr, Tele* tp) {
    int distX = curr->px - tp->x1;
    int distY = curr->py - tp->y1;
    distX = distX*distX;
    distY = distY*distY;
    int dist;
    dist = distX + distY;
    if (dist < 5) {
        next->px = tp->x2;
        next->py = tp->y2;
    }
}

void draw_mud(Mud* mud) {
    uLCD.filled_rectangle(mud->x1, mud->y1, mud->x2, mud->y2, BROWN);
}

void do_mud(int* coeff, Physics* curr, Mud* mud) {
    if (curr->px >= mud->x1 && curr->px <= mud->x2 && curr->py >= mud->y1 && curr->py <= mud->y2) {
        if (pb3.read() == 0) {
            *coeff = 0;
        } else if (pb4.read() == 0) {
            *coeff = 1;
        } else {
            *coeff = 3;
        }
    } else {
        if (pb3.read() == 0) {
            *coeff = 0;
        } else if (pb4.read() == 0) {
            *coeff = 5;
        } else {
            *coeff = 10;
        }
    }
}

/** Reads inputs to the game, such as accelerometer and buttons */
GameInputs read_inputs()
{
    GameInputs inputs = {0};
    
    // Read in the accelerometer data
    acc.readXYZGravity(&inputs.ax, &inputs.ay, &inputs.az);
    // Read in button data
    inputs.b1 = pb1.read();
    inputs.b2 = pb2.read();
    inputs.b3 = pb3.read();
    inputs.b4 = pb4.read();

    return inputs;
}

int update_game(DLinkedList* arena, Physics* curr, GameInputs inputs, float delta, DLinkedList* saves, int* coeff, int* coinCount)
{
    ///////////////////////////////
    // Prepare for physics update
    ///////////////////////////////
    int isDone = 0; 
    // Make a copy of the current state for modification
    Physics next = *curr;
    // No acceleration unless the ArenaElements apply them. (Newton's 1st law)
    next.ax = next.ay = 0;

    // Loop over all arena elements
    ArenaElement* elem = (ArenaElement*)getHead(arena);
    // Time for saving animations
    float time = .02;
    
    // Acceleration coefficient setting
    if (pb3.read() == 0) {
        *coeff = 0;
    }
    else if (pb4.read() == 0) {
        *coeff = 5;
    }
    
    do {
        switch(elem->type) {
            case WALL:
                do_wall(&next, curr, (Wall*) elem, delta);
                speaker = 0.0;
                break;
            case BALL:
                next.ax = inputs.ax;
                next.ay = inputs.ay;
                forward_euler(&next, delta, coeff);
                break;
            case POTHOLE:
                do_pothole(&next, curr, (Pothole*) elem);
                break;
            case TELE:
                do_tele(&next, curr, (Tele*) elem);
                break;
            case MUD:
                do_mud(coeff, curr, (Mud*) elem);
                break;
            case COIN:
                do_coin(coinCount, curr, (Coin*) elem);
                break;
            case FINISH:
                isDone = do_finish(&next, (Finish*) elem);
                break;
            default:
                break;
        }
    } while(elem = (ArenaElement*)getNext(arena));
    
    // Check if save button is pressed
    if (inputs.b1 == 0) {
        GameSave* save = (GameSave*) malloc(sizeof(GameSave));
        save->px = next.px;
        save->py = next.py;
        save->vx = next.vx;
        save->vy = next.vy;
        insertTail(saves, save);
        
        // Saving animation
        led1 = !led1;
        uLCD.locate(0, 0);
        uLCD.printf("Saving.   \r\n");
        wait(time);
        led1 = !led1;
        uLCD.locate(0, 0);
        uLCD.printf("Saving..  \r\n");
        wait(time);
        led1 = !led1;
        uLCD.locate(0, 0);
        uLCD.printf("Saving... \r\n");
        wait(time);
        led1 = !led1;
        led2 = 1;
    }
    
    // Check if recall button is pressed
    if (inputs.b2 == 0) {
        GameSave* saved = (GameSave*) getTail(saves);
        if (getSize(saves) != 0) { // Check if there is a save
            next.px = (float) saved->px;
            next.py = (float) saved->py;
            next.vx = (float) saved->vx;
            next.vy = (float) saved->vy;
            
            // Loading animation
            led1 = !led1;
            uLCD.locate(0, 0);
            uLCD.printf("Loading.   \r\n");
            wait(time);
            led1 = !led1;
            uLCD.locate(0, 0);
            uLCD.printf("Loading..  \r\n");
            wait(time);
            led1 = !led1;
            uLCD.locate(0, 0);
            uLCD.printf("Loading... \r\n");
            wait(time);
            led1 = !led1;
        }
        deleteBackward(saves);
    }
    
    // Check if there is a save
    if (getSize(saves) == 0) {
        led2 = 0;
    }
    
    // Check if two buttons are being pressed
    int pbSums = inputs.b1 + inputs.b2 + inputs.b3 + inputs.b4;
    if (pbSums == 2) {
        isDone = 1;
    }

    // Last thing! Update state, so it will be saved for the next iteration.
    *curr = next;
    
    // Zero means we aren't done yet
    return isDone;
}

int run_game(DLinkedList* arena, Physics* state)
{
    // Initialize game loop timers
    int tick, phys_tick, draw_tick;
    Timer timer;
    timer.start();
    tick = timer.read_ms();
    phys_tick = tick;
    draw_tick = tick;

    // Initialize debug counters
    int count = 0;
    int count2 = 0;

    // Initial draw of the game
    uLCD.background_color(DGREY);
    uLCD.cls();
    
    // Initialize saves
    DLinkedList* saves = create_dlinkedlist();

    // Speed coefficient
    int j = 1;
    int* coeff = &j;
    
    // Coin counter
    int b = 0;
    int* coinCount = &b;
    ///////////////////
    // Main game loop
    ///////////////////
    while(1) {
        // Read timer to determine how long the last loop took
        tick = timer.read_ms();
        
        ///////////////////
        // Physics Update
        ///////////////////
        // Rate limit: 1 ms
        int diff = tick - phys_tick;
        if (diff < 1) continue;
        phys_tick = tick;

        // Compute elapsed time in milliseconds
        float delta = diff*1e-3;

        // Read inputs
        GameInputs inputs = read_inputs();
        

        // Update game state
        int done = update_game(arena, state, inputs, delta, saves, coeff, coinCount);
        if (done) {
            currScore = currScore + timer.read_ms()/100 - *coinCount;
            destroyList(saves);
            return done;
        }
        

        // Debug: Count physics updates
        count2++;

        //////////////////
        // Render update
        //////////////////
        // Rate limit: 40ms
        if(tick - draw_tick < 40) continue;
        draw_tick = tick;

        // Erase moving stuff
        ArenaElement* elem = (ArenaElement*)getHead(arena);
        do {
            switch(elem->type) {
                case BALL:
                    erase_ball((Ball*) elem);
                    break;
                default: break;
            }
        } while(elem = (ArenaElement*)getNext(arena));

        // Draw everything
        elem = (ArenaElement*)getHead(arena);
        do {
            switch(elem->type) {
                case WALL:
                    draw_wall((Wall*) elem);
                    break;
                case BALL:
                    draw_ball((Ball*) elem, state);
                    break;
                case POTHOLE:
                    draw_pothole((Pothole*) elem);
                    break;
                case TELE:
                    draw_tele((Tele*) elem);
                    break;
                case MUD:
                    draw_mud((Mud*) elem);
                    break;
                case COIN:
                    draw_coin((Coin*) elem);
                    break;
                case FINISH:
                    draw_finish((Finish*) elem);
                    break;
                default:
                    break;
            }
        } while(elem = (ArenaElement*)getNext(arena));
        ///////////////
        // Debug info
        ///////////////
        // Displays rate info in the top corner
        //  First number is total time to update and render this frame
        //  Second number is how many physics iterations between drawing frames
        //  Only displayed every 10th render update (roughly 2.5 Hz)
        // TODO: Take this out before you turn your code in!
        int debug = 0;
        if ((debug == 1) && ((count = (count+1)%10) == 0)) {
            uLCD.locate(0, 0);
            uLCD.printf("%d %d \r\n", timer.read_ms()-tick, count2);
        } else if (debug == 0) {
            uLCD.locate(0, 0);
            uLCD.printf("Score: %d \r\n", timer.read_ms()/100 - *coinCount + currScore);
        }
        // Reset physics iteration counter after every render update
        count2 = 0;
    }
}