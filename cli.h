#ifndef CLI_H
#define CLI_H

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "minmax.h"
#include "core.h"


// Game states
#define MENU 0
#define GAME 1
#define GAME_OVER 2
#define SHOP 3
#define AI 4

// Shop colors
#define PLAYER_COLORS 4
#define UNLOCKABLE_COLORS (int[PLAYER_COLORS]){COLOR_RED, COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW}
#define PRICE 5

// Color pairs
#define RED_TEXT ((LANE_TYPES+2)*10)
#define COLOR_PAIR_LILY (WATER*10 + 9)

#define GAME_HEIGHT 20 // height of the displayed area
#define UNPLAYABLE_WIDTH 2 // width of the unplayable area on the sides of the screen

#define PLAYER_START_Y 5 // starting position of the player
#define AI_SPEED 10 // in frames, time it takes for the AI to move one tile

#define REFRESH_RATE 60 // refresh rate of the game in frames per second
#define GAME_SPEED 60 // in frames, time between each move of the camera

#define GRASS     1
#define WATER     2
#define TRACK     3
#define ROAD      4

void init_colors(void);
void display_lane(lane* lane, int lane_count);
void display(displayedData data);
void display_title_animation(void) ;
void display_shop(void);

#endif // CLI_H
