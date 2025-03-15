#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "core.h"

#define GAME_HEIGHT 20 // in tiles, height of the displayed area
#define REFRESH_RATE 60 // in frames per second, refresh rate of the game
#define GAME_SPEED 60 // in frames, time between each move of the camera

// Game states
#define MENU 0
#define GAME 1
#define GAME_OVER 2

// Color pairs
#define COLOR_PAIR_LILY 99

enum {
    COLOR_PAIR_GRASS = 1,
    COLOR_PAIR_WATER,
    COLOR_PAIR_TRACK,
    COLOR_PAIR_ROAD,
    COLOR_PAIR_GRASS_COIN,
    COLOR_PAIR_WATER_COIN,
    COLOR_PAIR_TRACK_COIN,
    COLOR_PAIR_ROAD_COIN,
    COLOR_PAIR_GRASS_PLAYER,
    COLOR_PAIR_WATER_PLAYER,
    COLOR_PAIR_TRACK_PLAYER,
    COLOR_PAIR_ROAD_PLAYER
};

void init_colors(int PLAYER_COLOR) {
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    start_color();
    init_pair(COLOR_PAIR_LILY, COLOR_GREEN, COLOR_BLUE);

    init_pair(COLOR_PAIR_GRASS, COLOR_BLACK, COLOR_GREEN);
    init_pair(COLOR_PAIR_WATER, COLOR_BLACK, COLOR_BLUE);
    init_pair(COLOR_PAIR_TRACK, COLOR_RED, COLOR_WHITE);
    init_pair(COLOR_PAIR_ROAD, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_PAIR_GRASS_COIN, COLOR_YELLOW, COLOR_GREEN);
    init_pair(COLOR_PAIR_WATER_COIN, COLOR_YELLOW, COLOR_BLUE);
    init_pair(COLOR_PAIR_TRACK_COIN, COLOR_YELLOW, COLOR_WHITE);
    init_pair(COLOR_PAIR_ROAD_COIN, COLOR_YELLOW, COLOR_BLACK);

    init_pair(COLOR_PAIR_GRASS_PLAYER, PLAYER_COLOR, COLOR_GREEN);
    init_pair(COLOR_PAIR_WATER_PLAYER, PLAYER_COLOR, COLOR_BLUE);
    init_pair(COLOR_PAIR_TRACK_PLAYER, PLAYER_COLOR, COLOR_WHITE);
    init_pair(COLOR_PAIR_ROAD_PLAYER, PLAYER_COLOR, COLOR_BLACK);
}

void display_lane(lane* lane, int lane_count) {
    attron(COLOR_PAIR(lane->type));

    int screen_y = GAME_HEIGHT - lane_count + 1; // + 1 to avoid displaying the lane on the first line
    for (int i = 0; i < LANE_WIDTH; i++) {
        mvprintw(screen_y, i, " ");
    }
    
    // Display the coins
    for (int i = 0; i < LANE_WIDTH; i++) {
        if (lane->coins[i]) {
            attron(COLOR_PAIR((lane->type) + LANE_TYPES));
            mvprintw(screen_y, i, "$");
            attroff(COLOR_PAIR((lane->type) + LANE_TYPES));
        }
    }

    // Display the obstacles

    if (lane->type == WATER && lane->speed == 0) {
            attron(COLOR_PAIR(COLOR_PAIR_LILY));
        }
    else {
        attron(COLOR_PAIR(lane->type));
    }

    obstacle* current_obstacle = lane->obstacles;

    while (current_obstacle != NULL) {
        for (int i = 0; i < current_obstacle->size && round(i+current_obstacle->x) < LANE_WIDTH; i++) {
            switch (lane->type)
            {
            case GRASS:
                mvprintw(screen_y, i+current_obstacle->x, "Y");       
                break;
            case WATER:
                if (lane->speed == 0) {
                    attroff(COLOR_PAIR(lane->type));
                    attron(COLOR_PAIR(COLOR_PAIR_LILY));
                    mvprintw(screen_y, i+current_obstacle->x, "0"); 
                    attroff(COLOR_PAIR(COLOR_PAIR_LILY));
                } else {
                    mvprintw(screen_y, round(i+current_obstacle->x), "=");   
                }   
                break;
            case TRACK:
                if (lane->speed > 0) {
                    mvprintw(screen_y, round(i+current_obstacle->x), ">");
                    if (current_obstacle->x + current_obstacle->size > -WARNING_TIME*TRAIN_SPEED && current_obstacle->x + current_obstacle->size < 0) {
                        mvprintw(screen_y, 0, "!");
                    }
                } else {
                    mvprintw(screen_y, round(i+current_obstacle->x), "<"); 
                }
                break;
            case ROAD:
                mvprintw(screen_y, round(i+current_obstacle->x), "#");   
                break;  
            default:
                break;
            }

        }
        // Warning for leftwards trains
        if (lane->type == TRACK && current_obstacle->x < LANE_WIDTH + WARNING_TIME*TRAIN_SPEED && current_obstacle->x > LANE_WIDTH) {
            mvprintw(screen_y, LANE_WIDTH - 1, "!");
        }
        current_obstacle = current_obstacle->next;
    }

    if (lane->type == WATER && lane->speed == 0) {
        attroff(COLOR_PAIR(COLOR_PAIR_LILY));
    }
    else {
        attroff(COLOR_PAIR(lane->type));
    }

}

void display(displayedData data) {
    // Display the lanes
    lane* current_lane = data.camera_first_lane;
    int lane_count = 0;
    while (lane_count <= GAME_HEIGHT) {
        display_lane(current_lane, lane_count);
        if (current_lane->y == data.player.y) {
            attron(COLOR_PAIR(current_lane->type + LANE_TYPES*2));
            mvprintw(round(data.cameraY - data.player.y + 1), round(data.player.x), "*");
            attroff(COLOR_PAIR(current_lane->type + LANE_TYPES*2));
        }
        current_lane = current_lane->next;
        lane_count++;
    }

    // Display the score
    mvprintw(0, 0, "Score: %d\n", data.player.y); // \n to not have issues going down from powers of 10
    refresh();
}

void display_title_animation() {
    const char* title[] = {
        "*********************",
        "*                   *",
        "*  ~ CROSSY ROAD ~  *",
        "*                   *",
        "*********************"
    };

    int title_length = sizeof(title) / sizeof(title[0]);

    for (int i = 0; i < title_length; i++) {
        mvprintw(GAME_HEIGHT/4 - 3 + i, 2, "%s", title[i]);
        refresh();
        nanosleep((const struct timespec[]){{0, 100000000L}}, NULL); // 100ms delay
    }
}

int main(void) {

    struct timespec remaining, request = { 0, 1000000000/REFRESH_RATE}; // ~1 frame at REFRESH_RATE fps

    srand(time(NULL));
    initscr();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    // Initialize colors
    init_colors(COLOR_RED);

    int game_state = MENU;
    int high_score = 0;
    int purse = 0;
    int move_timer;
    int current_y;
    int current_x;
    bool drown_flag;
    bool on_log = false;
    displayedData game;

    int menu_anim = true;
    

    while (true) {
        int ch = getch(); //Get the inputs from the keyboard

        switch (game_state) {

            case MENU:
            mvprintw(0, 0, "High score: %d", high_score);

            if (menu_anim) {
                display_title_animation();
                menu_anim = false;
            }

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
                on_log = false;
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

            // Camera movement, automatic or if player is in the top quarter of the game

            if (move_timer == 0 || game.cameraY - game.player.y < (GAME_HEIGHT/4)) { 
                game = move_camera(game);
                move_timer = GAME_SPEED;
            } else {
                move_timer--;
            }

            // Check if the player is colliding with something

            lane* current_lane = game.camera_first_lane;
            lane* player_lane;

            while (current_lane->next != NULL) {

                // Update lanes

                switch (current_lane->type) {
                    case ROAD:
                    update_vehicles(current_lane);
                    break;
                    case WATER:
                    update_drowning_slots(current_lane);
                    break;
                    case TRACK:
                    update_trains(current_lane);
                    break;
                    default:
                    break;
                }

                if (current_lane->y == game.player.y) {
                    player_lane = current_lane;
                }
                current_lane = current_lane->next;
            }

            if (player_lane != NULL) {

                // Check for collisions with coins
                for (int i = 0; i < LANE_WIDTH; i++) {
                    if (player_lane->coins[i] && game.player.x == i) {
                        purse++;
                        player_lane->coins[i] = false;
                    }
                }

                if (player_lane->type == WATER) {
                    drown_flag = true; // set this to false to disable drowning
                } else {
                    drown_flag = false;
                    on_log = false;
                    game.player.x = round(game.player.x);
                    game.player.y = round(game.player.y);
                }

                // Check for collisions with obstacles
                obstacle* collided_obstacle = collides(player_lane, game);
                if(collided_obstacle != NULL) {
                    switch (player_lane->type) {
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
                        if (player_lane->speed != 0) {
                            if (on_log) {
                                game.player.x += player_lane->speed;
                            } else {
                                on_log = true;
                                game.player.x = collided_obstacle->x + abs((int) (game.player.x - collided_obstacle->x));
                            }
                        }
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
            if (ch != ERR && ch != KEY_UP && ch != KEY_DOWN && ch != KEY_LEFT && ch != KEY_RIGHT) {
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
