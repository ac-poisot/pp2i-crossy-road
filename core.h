#pragma once
#include <stdbool.h>


#define LANE_WIDTH 24 // width of the displayed area
#define UNPLAYABLE_WIDTH 2 // width of the unplayable area on the sides of the screen

#define PLAYER_START_Y 5 // starting position of the player

#define LANE_TYPES 4 // amount of different types of lanes

#define GRASS     1
#define WATER     2
#define TRACK     3
#define ROAD      4

#define WARNING_TIME 60 // in frames, the time before the arrival of a train a warning is showed (! This value is taken into account in the spacing of the trains !)
#define TRAIN_LENGTH (LANE_WIDTH*3) // in tiles, length of a train
#define TRAIN_SPEED 1 // in tiles per frame, speed of a rightwards train
#define TRAIN_SPACING_MIN 100 // in tiles, minimum of space between two trains
#define TRAIN_SPACING_MAX 300 // in tiles, maximum of space between two trains

#define VEHICLE_SPEED_MIN 0.05 // in tiles per frame, minimum speed of a rightwards vehicle
#define VEHICLE_INTERVAL 6 // in tiles, space between two vehicles (either that amount or twice that amount)

#define LOG_SPEED 0.05 // in tiles per frame, speed of a rightwards log
#define MAX_LOG_SIZE 4 // in tiles, maximum size of a low
#define LOG_SPACING_MAX 5 // in tiles, maximum of space between two logs (minimum is always 1)

#define COIN_ISSUES 3 // there is a 1 in COIN_ISSUES chance of generating a coin


typedef struct obstacle {
    struct obstacle* next;
    struct obstacle* prev;
    float x; // position of the obstacle
    int size;
} obstacle;

typedef struct float_list {
    float val;
    struct float_list* next;
} float_list;

typedef struct lane {
    int y; // position of the lane
    float speed; // speed of the obstacles
    obstacle* obstacles; // array of obstacles present on the lane
    int obst_size; // size of the obstacle (log or vehicle length…)
    float_list* coins; // pointer to bools representing the position of coins present on the lane
    int type; // type of the lane (plains, road, tracks, river)
    struct lane* prev; // pointer to the previous lane
    struct lane* next; // pointer to the next lane
} lane;

typedef struct player {
    float y; // position of the player
    float x; // position of the player
    int orientation; // orientation of the player
    int skin; // skin of the player (?)
} player;

typedef struct displayedData {
    float cameraY; // camera position
    lane* first_lane; // pointer to the first lane of the game
    lane* camera_first_lane; // pointer to the first lane displayed on the screen
    player player; // player
    player ai; // articifial intelligence
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
void displayLanesToFile(lane* l, FILE* file);
void displayLanesToFile(lane* l, FILE* file);
obstacle* collides(lane *current_lane, displayedData game);
float_list* collides_coin(lane *current_lane, displayedData game);
displayedData move_camera(displayedData data, float speed);
displayedData init_game(int game_height);
