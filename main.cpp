/* Gatech ECE2035 2017 FALL MAZE RUNNER
* Copyright (c) 2017 Gatech ECE2035
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

// Include header files for platform
#include "mbed.h"

// Include header files for project
#include "globals.h"
#include "math_extra.h"
#include "physics.h"
#include "game.h"
#include "wall.h"
#include "doublely_linked_list.h"
#include <stdlib.h>

// Hardware initialization
DigitalIn pb1(p21);  // push button
DigitalIn pb2(p22);  // push button
DigitalIn pb3(p23);  // push button
DigitalIn pb4(p24);  // push button
uLCD_4DGL uLCD(p9,p10,p11); // LCD (serial tx, serial rx, reset pin;)
SDFileSystem sd(p5, p6, p7, p8, "sd"); // SD card and filesystem (mosi, miso, sck, cs)
PwmOut speaker(p25); // Speaker PWM initialization
DigitalOut led1(LED1); // Led 1
DigitalOut led2(LED2); // Led 2
DigitalOut led3(LED3); // Led 3
DigitalOut led4(LED4); // Led 4

// Level creation method declaration
DLinkedList* create_arenaOne();
DLinkedList* create_arenaTwo();
DLinkedList* create_arenaThree();

// Parameters. Declared in globals.h
const float mass = 0.001;
const int radius = 4;
const float bounce=0.5;
int minScore = 300;
int currScore = 0;

/** Main() is where you start your implementation */
int main()
{
    ////////////////////////////
    // Power-on initialization
    ////////////////////////////
    // Turn up the serial data rate so we don't lag
    uLCD.baudrate(3000000);
    pc.baud(115200);

    // Initialize the buttons
    // Each pb* variable is 1 when pressed
    pb1.mode(PullUp);
    pb2.mode(PullUp);
    pb3.mode(PullUp);
    pb4.mode(PullUp);
    
    acc.activate();

    ///////////////
    // Reset loop
    ///////////////
    // This is where control between major phases of the game happens
    // This is a good place to add choose levels, add the game menu, etc.
    while(1) 
    {
        
        // MENU 
        //text
        int loop2 = 1;
        while(loop2) {
            uLCD.background_color(WHITE);
            uLCD.cls();
            uLCD.textbackground_color(WHITE);
            uLCD.set_font(FONT_7X8);
            uLCD.text_mode(TRANSPARENT);
            uLCD.text_width(2); //4X size text
            uLCD.text_height(2);
            uLCD.color(BROWN);
            uLCD.printf("\nMain Menu:\n");
            uLCD.text_width(1);
            uLCD.text_height(1);
            uLCD.color(BLUE);
            uLCD.printf("\n\n(1) Play");
            uLCD.printf("\n\n(2) Highscore");
            
            int loop = 1; // loop variable
            while (loop) {
                if (pb1 == 0) {
                    loop = 0;
                    loop2 = 0;
                } else if (pb2 == 0) {
                    uLCD.cls();
                    uLCD.text_width(2); //4X size text
                    uLCD.text_height(2);
                    uLCD.color(BROWN);
                    uLCD.printf("\nRecord:\n %d", minScore);
                    uLCD.text_width(1);
                    uLCD.text_height(1);
                    uLCD.color(BLUE);
                    uLCD.printf("\n\n\n\n\n(4) Return\n");
                    int loop3 = 1;
                    while (loop3) {
                        if (pb4 == 0) {
                            loop = 0;
                            loop3 = 0;
                        }
                    }
                }
            }
        }
        
        
        
        
        // Initialze game state
        Physics state = {0};
        state.px = 8;        // Position in the center of the screen
        state.py = 120;
        state.vx = 0.0;         // Initially unmoving
        state.vy = 0.0;

        // Delegate to the game loop to execute the level
        // run_game() is in game.cpp
        DLinkedList* arenaOne = create_arenaOne();
        DLinkedList* arenaTwo = create_arenaTwo();
        DLinkedList* arenaThree = create_arenaThree();
        
        int sum = 0; // Sum of pushbuttons
        run_game(arenaOne, &state);
        while(sum != 4) {
            /* This ensures that two buttons are not pressed
            so that it does not immediately skip all levels from pressing two
            buttons one time. */
            sum = pb1.read() + pb2.read() + pb3.read() + pb4.read();
        }
        state.px = 8;        // Position in the center of the screen
        state.py = 120;
        state.vx = 0.0;         // Initially unmoving
        state.vy = 0.0;
        run_game(arenaTwo, &state);
        sum = 0;
        while(sum != 4) {
            sum = pb1.read() + pb2.read() + pb3.read() + pb4.read();
        }
        state.px = 8;        // Position in the center of the screen
        state.py = 120;
        state.vx = 0.0;         // Initially unmoving
        state.vy = 0.0;
        run_game(arenaThree, &state);
        sum = 0;
        while(sum != 4) {
            sum = pb1.read() + pb2.read() + pb3.read() + pb4.read();
        }
        
        uLCD.background_color(BLACK);
        uLCD.cls();
        uLCD.textbackground_color(BLACK);
        uLCD.set_font(FONT_7X8);
        uLCD.text_mode(TRANSPARENT);
        uLCD.text_width(2); //4X size text
        uLCD.text_height(2);
        uLCD.text_italic(1);
       // uLCD.locate(20, 45);
        uLCD.printf("\n\n\n You Win!\n");
        
        if (minScore > currScore) {
                minScore = currScore;
        }
        currScore = 0;
        
        // Destory the arena and entities once we're done with the level
        destroyList(arenaOne);
        destroyList(arenaTwo);
        destroyList(arenaThree);
        
        // Loop on finish until 1 button is pressed
        int i = 0;
        while ((pb1.read() + pb2.read() + pb3.read() + pb4.read()) > 3) {
            if (i%6 == 0) {
                led1 = 1;
                led2 = 0;
                wait(.1);
            } else if (i%6 == 1) {
                led1 = 0;
                led2 = 1;
                wait(.1);
            }
            else if (i%6 == 2) {
                led2 = 0;
                led3 = 1;
                wait(.1);
            }
            else if (i%6 == 3) {
                led4 = 1;
                led3 = 0;
                wait(.1);
            }
            else if (i%6 == 4) {
                led3 = 1;
                led4 = 0;
                wait(.1);
            }
            else if (i%6 == 5) {
                led2 = 1;
                led3 = 0;
                wait(.1);
            }
            i++;
        }
    }
}

DLinkedList* create_arenaTwo()
{
DLinkedList* arena = create_dlinkedlist();
    
    // Initialize the mud
    Mud* mud = (Mud*) malloc(sizeof(Mud));
    mud->type = MUD;
    mud->x1 = 0;
    mud->y1 = 47;
    mud->x2 = 40;
    mud->y2 = 80;
    // Add it to list
    insertTail(arena, (void*)mud);

    // Initialize the walls
    Wall* walls[7];
    walls[0] = create_wall(HORIZONTAL, 0, 0, 127, bounce);  // top
    walls[1] = create_wall(HORIZONTAL, 0, 127, 127, bounce);// bottom
    walls[2] = create_wall(VERTICAL, 0, 0, 127, bounce);    // left
    walls[3] = create_wall(VERTICAL, 127, 0, 127, bounce);  // right
    walls[4]  = create_wall(HORIZONTAL, 0, 35, 80, bounce);
    walls[5] = create_wall(HORIZONTAL, 47, 60, 80, bounce);
    walls[6] = create_wall(HORIZONTAL, 0, 95, 80, bounce);
    
    // Add the walls to the arena
    for (int i = 0; i < 7; i++)
        insertTail(arena, (void*)walls[i]);
        
    // Initialize the finish area
    Finish* finish = (Finish*) malloc(sizeof(Finish));
    finish->type = FINISH;
    finish->x = 8;
    finish->y = 15;
    // Add it to list
    insertTail(arena, (void*)finish);
    
    // Initialize the coins
    Coin* coin = (Coin*) malloc(sizeof(Coin));
    coin->type = COIN;
    coin->x = 30;
    coin->y = 40;
    coin->draw = 1;
    insertTail(arena, (void*)coin);
    
    Coin* coin1 = (Coin*) malloc(sizeof(Coin));
    coin1->type = COIN;
    coin1->x = 40;
    coin1->y = 52;
    coin1->draw = 1;
    insertTail(arena, (void*)coin1);
    
    
    Coin* coin2 = (Coin*) malloc(sizeof(Coin));
    coin2->type = COIN;
    coin2->x = 30;
    coin2->y = 80;
    coin2->draw = 1;
    insertTail(arena, (void*)coin2);
    
    // Initialize the teleporter
    Tele* tp = (Tele*) malloc(sizeof(Tele));
    tp->type = TELE;
    tp->x1 = 120;
    tp->y1 = 120;
    tp->x2 = 30;
    tp->y2 = 15;
    // Add it to list
    insertTail(arena, (void*)tp);
    
    // Initialize the potholes
    Pothole* ph1 = (Pothole*) malloc(sizeof(Pothole));
    ph1->type = POTHOLE;
    ph1->x = 107;
    ph1->y = 120;
    ph1->r = 6;
    insertTail(arena, (void*)ph1);
    
    Pothole* ph2 = (Pothole*) malloc(sizeof(Pothole));
    ph2->type = POTHOLE;
    ph2->x = 120;
    ph2->y = 50;
    ph2->r = 6;
    insertTail(arena, (void*)ph2);

    // Initialize the ball
    Ball* ball = (Ball*) malloc(sizeof(Ball));
    ball->type = BALL;
    ball->x = 8;
    ball->y = 120;
    
    // Add ball to the arena 
    // NOTE: The ball should always be last in the arena list, so that the other 
    // ArenaElements have a chance to compute the Physics updates before the 
    // ball applies forward euler method.
    insertTail(arena, (void*)ball);

    return arena;
}

DLinkedList* create_arenaOne()
{
    DLinkedList* arena = create_dlinkedlist();
    
    // Initialize the mud
    Mud* mud = (Mud*) malloc(sizeof(Mud));
    mud->type = MUD;
    mud->x1 = 0;
    mud->y1 = 47;
    mud->x2 = 50;
    mud->y2 = 60;
    // Add it to list
    insertTail(arena, (void*)mud);

    // Initialize the walls
    Wall* walls[7];
    walls[0] = create_wall(HORIZONTAL, 0, 0, 127, bounce);  // top
    walls[1] = create_wall(HORIZONTAL, 0, 127, 127, bounce);// bottom
    walls[2] = create_wall(VERTICAL, 0, 0, 127, bounce);    // left
    walls[3] = create_wall(VERTICAL, 127, 0, 127, bounce);  // right
    walls[4]  = create_wall(HORIZONTAL, 0, 40, 80, bounce);
    walls[5] = create_wall(HORIZONTAL, 47, 64, 80, bounce);
    walls[6] = create_wall(HORIZONTAL, 0, 90, 80, bounce);
    
    // Add the walls to the arena
    for (int i = 0; i < 7; i++)
        insertTail(arena, (void*)walls[i]);
        
    // Initialize the finish area
    Finish* finish = (Finish*) malloc(sizeof(Finish));
    finish->type = FINISH;
    finish->x = 8;
    finish->y = 15;
    // Add it to list
    insertTail(arena, (void*)finish);
    
    // Initialize the coins
    
    Coin* coin1 = (Coin*) malloc(sizeof(Coin));
    coin1->type = COIN;
    coin1->x = 40;
    coin1->y = 52;
    coin1->draw = 1;
    insertTail(arena, (void*)coin1);
    
    Coin* coin3 = (Coin*) malloc(sizeof(Coin));
    coin3->type = COIN;
    coin3->x = 40;
    coin3->y = 65;
    coin3->draw = 1;
    insertTail(arena, (void*)coin3);
    
    
    // Initialize the potholes
    Pothole* ph1 = (Pothole*) malloc(sizeof(Pothole));
    ph1->type = POTHOLE;
    ph1->x = 50;
    ph1->y = 120;
    ph1->r = 6;
    insertTail(arena, (void*)ph1);
    
    Pothole* ph2 = (Pothole*) malloc(sizeof(Pothole));
    ph2->type = POTHOLE;
    ph2->x = 120;
    ph2->y = 107;
    ph2->r = 6;
    insertTail(arena, (void*)ph2);

    // Initialize the ball
    Ball* ball = (Ball*) malloc(sizeof(Ball));
    ball->type = BALL;
    ball->x = 8;
    ball->y = 120;
    
    // Add ball to the arena 
    // NOTE: The ball should always be last in the arena list, so that the other 
    // ArenaElements have a chance to compute the Physics updates before the 
    // ball applies forward euler method.
    insertTail(arena, (void*)ball);

    return arena;
}

DLinkedList* create_arenaThree()
{
    DLinkedList* arena = create_dlinkedlist();
    
    // Initialize the mud
    Mud* mud = (Mud*) malloc(sizeof(Mud));
    mud->type = MUD;
    mud->x1 = 0;
    mud->y1 = 47;
    mud->x2 = 40;
    mud->y2 = 80;
    // Add it to list
    insertTail(arena, (void*)mud);

    // Initialize the walls
    Wall* walls[7];
    walls[0] = create_wall(HORIZONTAL, 0, 0, 127, bounce);  // top
    walls[1] = create_wall(HORIZONTAL, 0, 127, 127, bounce);// bottom
    walls[2] = create_wall(VERTICAL, 0, 0, 127, bounce);    // left
    walls[3] = create_wall(VERTICAL, 127, 0, 127, bounce);  // right
    walls[4]  = create_wall(HORIZONTAL, 0, 27, 80, bounce);
    walls[5] = create_wall(HORIZONTAL, 47, 64, 80, bounce);
    walls[6] = create_wall(HORIZONTAL, 0, 100, 80, bounce);
    
    // Add the walls to the arena
    for (int i = 0; i < 7; i++)
        insertTail(arena, (void*)walls[i]);
        
    // Initialize the finish area
    Finish* finish = (Finish*) malloc(sizeof(Finish));
    finish->type = FINISH;
    finish->x = 8;
    finish->y = 15;
    // Add it to list
    insertTail(arena, (void*)finish);
    
    // Initialize the coins
    Coin* coin = (Coin*) malloc(sizeof(Coin));
    coin->type = COIN;
    coin->x = 30;
    coin->y = 40;
    coin->draw = 1;
    insertTail(arena, (void*)coin);
    
    Coin* coin1 = (Coin*) malloc(sizeof(Coin));
    coin1->type = COIN;
    coin1->x = 40;
    coin1->y = 52;
    coin1->draw = 1;
    insertTail(arena, (void*)coin1);
    
    Coin* coin3 = (Coin*) malloc(sizeof(Coin));
    coin3->type = COIN;
    coin3->x = 40;
    coin3->y = 65;
    coin3->draw = 1;
    insertTail(arena, (void*)coin3);
    
    Coin* coin2 = (Coin*) malloc(sizeof(Coin));
    coin2->type = COIN;
    coin2->x = 30;
    coin2->y = 80;
    coin2->draw = 1;
    insertTail(arena, (void*)coin2);
    
    // Initialize the teleporter
    Tele* tp = (Tele*) malloc(sizeof(Tele));
    tp->type = TELE;
    tp->x1 = 120;
    tp->y1 = 120;
    tp->x2 = 30;
    tp->y2 = 15;
    // Add it to list
    insertTail(arena, (void*)tp);
    
    // Initialize the potholes
    Pothole* ph1 = (Pothole*) malloc(sizeof(Pothole));
    ph1->type = POTHOLE;
    ph1->x = 107;
    ph1->y = 120;
    ph1->r = 6;
    insertTail(arena, (void*)ph1);
    
    Pothole* ph2 = (Pothole*) malloc(sizeof(Pothole));
    ph2->type = POTHOLE;
    ph2->x = 120;
    ph2->y = 107;
    ph2->r = 6;
    insertTail(arena, (void*)ph2);
    
    Pothole* ph = (Pothole*) malloc(sizeof(Pothole));
    ph->type = POTHOLE;
    ph->x = 50;
    ph->y = 50;
    ph->r = 5;
    insertTail(arena, (void*)ph);

    // Initialize the ball
    Ball* ball = (Ball*) malloc(sizeof(Ball));
    ball->type = BALL;
    ball->x = 8;
    ball->y = 120;
    
    // Add ball to the arena 
    // NOTE: The ball should always be last in the arena list, so that the other 
    // ArenaElements have a chance to compute the Physics updates before the 
    // ball applies forward euler method.
    insertTail(arena, (void*)ball);

    return arena;
}