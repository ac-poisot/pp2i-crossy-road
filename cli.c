#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include "core.h"

#define GAME_HEIGHT 20 // height of the displayed area


#define REFRESH_RATE 60 // refresh rate of the game in frames per second
#define GAME_SPEED 20 // in frames, time between each move of the camera

// Game states
#define MENU 0
#define GAME 1
#define GAME_OVER 2


void display_lane(lane* lane, int lane_count) {
    attron(COLOR_PAIR(lane->type));

    int screen_y = GAME_HEIGHT - lane_count + 1; // + 1 to avoid displaying the lane on the first line
    for (int i = 0; i < LANE_WIDTH; i++) {
        mvprintw(screen_y, i, " ");
    }
    
    // Display the coins
    for (int i = 0; i < LANE_WIDTH; i++) {
        if (lane->coins[i]) {
            attron(COLOR_PAIR((lane->type) + 4));
            mvprintw(screen_y, i, "$");
            attroff(COLOR_PAIR((lane->type) + 4));
        }
    }

    // Display the obstacles
    attron(COLOR_PAIR(lane->type));

    obstacle* current_obstacle = lane->obstacles;

    while (current_obstacle != NULL) {
        for (int i = 0; i < lane->obst_size && i+current_obstacle->x < LANE_WIDTH; i++) {
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


int main(void) {

    struct timespec remaining, request = { 0, 1000000000/REFRESH_RATE}; // ~1 frame at REFRESH_RATE fps


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
    bool drown_flag;
    displayedData game;

    while (true) {
        int ch = getch(); //Get the inputs from the keyboard

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
                game = init_game(GAME_HEIGHT);
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

                if (current_lane->type == WATER) {
                    drown_flag = true; // set this to false to disable drowning
                } else {
                    drown_flag = false;
                }

                // Check for collisions with obstacles
                if(collides(current_lane, game)) {
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
                        drown_flag = false;
                        break;
                        break;
                        default:
                        break;
                    }
                }

            }

            if (game.player.y < game.cameraY - GAME_HEIGHT || drown_flag) {
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
        nanosleep(&request, &remaining); 
    }

    endwin();
    return 0;
}
