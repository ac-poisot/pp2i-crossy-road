#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
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

typedef struct obstacle {
    struct obstacle* next;
    struct obstacle* prev;
    float x; // position of the obstacle
    int size; // size of the obstacle (log or vehicle length…)
} obstacle;

typedef struct lane {
    int y; // position of the lane
    float speed; // speed of the obstacles
    obstacle* obstacles; // array of obstacles present on the lane
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

void display_lane(lane* lane, int lane_count) {
    attron(COLOR_PAIR(lane->type));

    int screen_y = GAME_HEIGHT - lane_count + 1; // + 1 to avoid displaying the lane on the first line
    for (int i = 0; i < LANE_WIDTH; i++) {
        mvprintw(screen_y, i, " ");
    }
    //Pas d'obstacles actuellement
    // Display the obstacles
    obstacle* current_obstacle = lane->obstacles;

    while (current_obstacle != NULL) {
        for (int i = 0; i < current_obstacle->size; i++) {
            switch (lane->type)
            {
            case GRASS:
                mvprintw(screen_y, i+current_obstacle->x, "T");       
                break;
            case WATER:
                mvprintw(screen_y, i+current_obstacle->x, "O");      
                break;
            case TRACK:
                mvprintw(screen_y, i+current_obstacle->x, ">");        
                break;
            case ROAD:
                mvprintw(screen_y, i+current_obstacle->x, "V");   
                break;  
            default:
                break;
            }
        }
        current_obstacle = current_obstacle->next;
    }
    
    // Display the coins
    for (int i = 0; i < LANE_WIDTH; i++) {
        if (lane->coins[i]) {
            attron(COLOR_PAIR((lane->type) + 4));
            mvprintw(screen_y, i, "$");
            attroff(COLOR_PAIR((lane->type) + 4));
        }
    }

    attroff(COLOR_PAIR(lane->type));
}

void display(displayedData data) {
    // Display the lanes
    lane* current_lane = data.camera_first_lane;
    int lane_count = 0;
    while (lane_count <= GAME_HEIGHT) {
        display_lane(current_lane, lane_count);
        current_lane = current_lane->next;
        lane_count++;
    }

    // Display the player
    mvprintw(data.cameraY - data.player.y + 1, data.player.x, "P");
    // Display the score
    mvprintw(0, 0, "Score: %d", data.player.y);
    refresh();
}

void free_lanes(lane* first_lane) {
    lane* current_lane = first_lane;
    lane* next_lane;
    while (current_lane != NULL) {
        next_lane = current_lane->next;
        free(current_lane->coins);
        free(current_lane);
        current_lane = next_lane;
    }
}

lane* random_lane(lane* prev_lane) {
    lane* new_lane = malloc(sizeof(lane));
    new_lane->y = prev_lane->y + 1;
    new_lane->speed = 0;

    // Only one obstacle, for testing purposes
    new_lane->obstacles = malloc(sizeof(obstacle));
    new_lane->obstacles->next = NULL;
    new_lane->obstacles->prev = NULL;
    new_lane->obstacles->x = rand() % LANE_WIDTH;
    new_lane->obstacles->size = 1;


    new_lane->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        new_lane->coins[i] = !(rand() % 30); // for each tile, 1/30 chance of having a coin
    }
    new_lane->type = (rand() % 4) + 1;
    new_lane->prev = prev_lane;
    prev_lane->next = new_lane;
    new_lane->next = NULL;
    return new_lane;
}

displayedData move_camera(displayedData data) {
    // Move the camera
    data.cameraY++;
    data.camera_first_lane = data.camera_first_lane->next;

    // Add a new lane
    lane* current_lane = data.camera_first_lane;
    while (current_lane->next != NULL) {
        current_lane = current_lane->next;
    }
    random_lane(current_lane);

    return data;
}

displayedData init_game(void) {
    player p = {PLAYER_START_Y, LANE_WIDTH/2, 0, 0};

    // Initialize the first lane
    lane* l = malloc(sizeof(lane));
    l->y = 0;
    l->obstacles = NULL;
    l->coins = malloc(LANE_WIDTH * sizeof(bool));
    for (int i = 0; i < LANE_WIDTH; i++) {
        l->coins[i] = false;
    }
    l->type = (rand() % 4) + 1;
    l->prev = NULL;
    l->next = NULL;

    lane* first_lane = l;

    // Generate the first lanes
    for (int i = 0; i < GAME_HEIGHT + 5; i++) {
        l = random_lane(l);
    }

    displayedData res = {GAME_HEIGHT, first_lane, first_lane, p, 0, 0};

    return res;
}

int main(void) {
    srand(time(NULL));
    initscr();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    // Initialize colors
    start_color();
    init_pair(GRASS, COLOR_BLACK, COLOR_GREEN);
    init_pair(WATER, COLOR_GREEN, COLOR_BLUE);
    init_pair(TRACK, COLOR_RED, COLOR_WHITE);
    init_pair(ROAD, COLOR_MAGENTA, COLOR_BLACK);

    // Initialize coin colors
    init_pair(GRASS + 4, COLOR_YELLOW, COLOR_GREEN);
    init_pair(WATER + 4, COLOR_YELLOW, COLOR_BLUE);
    init_pair(TRACK + 4, COLOR_YELLOW, COLOR_WHITE);
    init_pair(ROAD + 4, COLOR_YELLOW, COLOR_BLACK);

    int game_state = MENU;
    int high_score = 0;
    int purse = 0;
    int move_timer;
    int current_y;
    int current_x;
    displayedData game;

    while (true) {
        int ch = getch();//Get the inputs from the keyboard

        switch (game_state) {

            case MENU:
            mvprintw(0, 0, "High score: %d", high_score);
            mvprintw(GAME_HEIGHT/4, 5, "~ CROSSY ROAD :3 ~");
            mvprintw(GAME_HEIGHT/4 + 3, 3, "Press any key to play");
            mvprintw(GAME_HEIGHT/4 + 4, 7, "Press q to quit");
            if (ch == 'q') {
                endwin();
                return 0;
            }
            if (ch != ERR) {
                clear();
                game_state = GAME;
                move_timer = GAME_SPEED;
                game = init_game();
            }
            break;

            case GAME:

            current_y = game.player.y;
            current_x = game.player.x;

            switch (ch) {
                case KEY_UP:
                if (game.player.y < game.cameraY) {
                    game.player.y++;
                }
                break;
                case KEY_DOWN:
                game.player.y--;
                break;
                case KEY_LEFT:
                if (game.player.x > UNPLAYABLE_WIDTH)
                game.player.x--;
                break;
                case KEY_RIGHT:
                if (game.player.x < LANE_WIDTH - UNPLAYABLE_WIDTH - 1) {
                    game.player.x++;
                }
                break;
                default:
                break;
            }
    
            if (move_timer == 0) {
                game = move_camera(game);
                move_timer = GAME_SPEED;
            } else {
                move_timer--;
            }
        

            // Check if the player is colliding with something

            lane* current_lane = game.camera_first_lane;

            while (current_lane->y != game.player.y && current_lane->next != NULL) {
                current_lane = current_lane->next;
            }

            if (current_lane != NULL) {

                // Check for collisions with coins
                for (int i = 0; i < LANE_WIDTH; i++) {
                    if (current_lane->coins[i] && game.player.x == i) {
                        purse++;
                        current_lane->coins[i] = false;
                    }
                }

                // Check for collisions with obstacles
                obstacle* current_obstacle = current_lane->obstacles;
                while (current_obstacle != NULL) {
                    if (game.player.x >= current_obstacle->x && game.player.x < current_obstacle->x + current_obstacle->size) {
                        switch (current_lane->type) {
                            case GRASS:
                            game.player.x = current_x;
                            game.player.y = current_y;
                            break;
                            case TRACK:
                            game_state = GAME_OVER;
                            break;
                            case ROAD:
                            game_state = GAME_OVER;
                            break;
                            case WATER:
                            break;
                            break;
                            default:
                            break;
                        }
                    }
                    current_obstacle = current_obstacle->next;
                }

            }

            if (game.player.y < game.cameraY - GAME_HEIGHT) {
                game_state = GAME_OVER;
            }

            if (game_state != GAME_OVER) {
                display(game);
                // Display the purse
                mvprintw(0, LANE_WIDTH-3, "%d$", purse);
                refresh();
            }
        
            break;

            case GAME_OVER:
            mvprintw(0, 0, "Final score: %d", game.player.y);
            mvprintw(GAME_HEIGHT/4, LANE_WIDTH+2, "GAME OVER ;-;");
            mvprintw(GAME_HEIGHT/4 + 3, LANE_WIDTH+2, "Press any key to return to menu");
            if (ch != ERR) {
                clear();
                game_state = MENU;
                if (game.player.y > high_score) {
                    high_score = game.player.y;
                }
                free_lanes(game.first_lane);
            }
            break;
            default:
            mvprintw(0, 0, "ERROR");
            break;
        }

        usleep(1000000 / REFRESH_RATE); // ~1 frame at REFRESH_RATE fps
    }

    endwin();
    return 0;
}