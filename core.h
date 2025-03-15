#pragma once
#include <stdbool.h>


#define LANE_WIDTH 30 // width of the displayed area
#define UNPLAYABLE_WIDTH 2 // width of the unplayable area on the sides of the screen

#define PLAYER_START_Y 5 // starting position of the player

#define LANE_TYPES 4

#define GRASS     1
#define WATER     2
#define TRACK     3
#define ROAD      4

#define TRAIN_LENGTH 10
#define TRAIN_SPEED 2
#define TRAIN_SPACING_MIN 200
#define TRAIN_SPACING_MAX 500

#define VEHICLE_SPEED_MIN 0.05
#define VEHICLE_SPACING_MIN 6
#define VEHICLE_SPACING_MAX 12

#define LOG_SPEED 0.05
#define MAX_LOG_SIZE 4
#define LOG_SPACING_MAX 5


typedef struct obstacle {
    struct obstacle* next;
    struct obstacle* prev;
    float x; // position of the obstacle
    int size;
} obstacle;

typedef struct lane {
    int y; // position of the lane
    float speed; // speed of the obstacles
    obstacle* obstacles; // array of obstacles present on the lane
    int obst_size; // size of the obstacle (log or vehicle length…)
    bool* coins; // pointer to bools representing the position of coins present on the lane
    int type; // type of the lane (plains, road, tracks, river)
    struct lane* prev; // pointer to the previous lane
    struct lane* next; // pointer to the next lane
} lane;

typedef struct player {
    int y; // position of the player
    float x; // position of the player
    int orientation; // orientation of the player
    int skin; // skin of the player (?)
} player;

typedef struct displayedData {
    float cameraY; // camera position
    lane* first_lane; // pointer to the first lane of the game
    lane* camera_first_lane; // pointer to the first lane displayed on the screen
    player player; // player
    int score; // score
    int gameOver; // 1 if the game is over, 0 otherwise
} displayedData;



void free_obstacles(obstacle* first_obstacle);
void free_lanes(lane* first_lane);
int biome(int* boules);
int* probabilite_biomes(lane* l);
lane* empty_lane(lane* prev_lane, int type);
lane* initialLanes(void);
lane* random_lane(lane* prev_lane);
obstacle* generate_vehicles(lane* l);
obstacle* generate_trees(void);
obstacle* generate_waterlilies(void);
obstacle* generate_drowning_slots(void);
obstacle* generate_trains(void);
lane* generate_lane(lane* prev_lane, int type);
void update_vehicles(lane* l);
void update_drowning_slots(lane* l);
void update_trains(lane* l);
void display_obstacles(obstacle* l);
void displayLanes(lane* l);
obstacle* collides(lane *current_lane, displayedData game);
displayedData move_camera(displayedData data);
displayedData init_game(int game_height);
