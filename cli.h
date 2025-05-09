#ifndef CLI_H
#define CLI_H

#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>

#define LANE_WIDTH 20 // width of the displayed area
#define GAME_HEIGHT 20 // height of the displayed area
#define UNPLAYABLE_WIDTH 2 // width of the unplayable area on the sides of the screen

#define PLAYER_START_Y 5 // starting position of the player

#define REFRESH_RATE 60 // refresh rate of the game in frames per second
#define GAME_SPEED 20 // in frames, time between each move of the camera

#define GRASS     1
#define WATER     2
#define TRACK     3
#define ROAD      4

// Game states
#define MENU 0
#define GAME 1
#define GAME_OVER 2

//typedef struct obstacle {
//    struct obstacle* next;
//    struct obstacle* prev;
//    float x; // position of the obstacle
//    int size; // size of the obstacle (log or vehicle length…)
//} obstacle;
//
//typedef struct lane {
//    int y; // position of the lane
//    float speed; // speed of the obstacles
//    obstacle* obstacles; // array of obstacles present on the lane
//    bool* coins; // pointer to bools representing the position of coins present on the lane
//    int type; // type of the lane (plains, road, tracks, river)
//    struct lane* prev; // pointer to the previous lane
//    struct lane* next; // pointer to the next lane
//} lane;
//
//typedef struct player {
//    int y; // position of the player
//    float x; // position of the player
//    int orientation; // orientation of the player
//    int skin; // skin of the player (?)
//} player;
//
//typedef struct displayedData {
//    float cameraY; // camera position
//    lane* first_lane; // pointer to the first lane of the game
//    lane* camera_first_lane; // pointer to the first lane displayed on the screen
//    player player; // player
//    int score; // score
//    int purse; // purse
//    int gameOver; // 1 if the game is over, 0 otherwise
//} displayedData;

void init_colors(void);
void display_lane(lane* lane, int lane_count);
void display(displayedData data);
void display_title_animation(void) ;
void display_shop(void);

#endif // CLI_H