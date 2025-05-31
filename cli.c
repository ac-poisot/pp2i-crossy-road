#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "core.h"
#include "minmax.h"

#define GAME_HEIGHT 20 // in tiles, height of the displayed area
#define REFRESH_RATE 60 // in frames per second, refresh rate of the game
#define GAME_SPEED 60 // in frames, time between each move of the camera

#define AI_SPEED 20
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

void init_colors() {
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    start_color();
    init_pair(RED_TEXT, COLOR_RED, COLOR_BLACK);

    init_pair(COLOR_PAIR_LILY, COLOR_GREEN, COLOR_BLUE);

    init_pair(GRASS*10, COLOR_BLACK, COLOR_GREEN);
    init_pair(WATER*10, COLOR_BLACK, COLOR_BLUE);
    init_pair(TRACK*10, COLOR_RED, COLOR_WHITE);
    init_pair(ROAD*10, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(1+GRASS*10, COLOR_YELLOW, COLOR_GREEN);
    init_pair(1+WATER*10, COLOR_YELLOW, COLOR_BLUE);
    init_pair(1+TRACK*10, COLOR_YELLOW, COLOR_WHITE);
    init_pair(1+ROAD*10, COLOR_YELLOW, COLOR_BLACK);



    for (int i=0; i<PLAYER_COLORS; i++) {
        init_pair((LANE_TYPES+1)*10+i, UNLOCKABLE_COLORS[i], COLOR_BLACK);

        init_pair(10*GRASS+(i+2), UNLOCKABLE_COLORS[i], COLOR_GREEN);
        init_pair(10*WATER+(i+2), UNLOCKABLE_COLORS[i], COLOR_BLUE);
        init_pair(10*TRACK+(i+2), UNLOCKABLE_COLORS[i], COLOR_WHITE);
        init_pair(10*ROAD+(i+2), UNLOCKABLE_COLORS[i], COLOR_BLACK);
    }
}


void display_lane(lane* lane, int lane_count) {
    attron(COLOR_PAIR(lane->type*10 + 1));

    int screen_y = GAME_HEIGHT - lane_count + 1; // + 1 to avoid displaying the lane on the first line
    for (int i = 0; i < LANE_WIDTH; i++) {
        mvprintw(screen_y, i, " ");
    }
    
    // Display the coins
    float_list *coins = lane->coins;
    if (lane->type != WATER) {
        while(coins != NULL) {
            if (round(coins->val) < LANE_WIDTH && round(coins->val) >= 0) {
                mvprintw(screen_y, (int)round(coins->val), "$");
            }
            coins = coins->next;
        }
    }

    attroff(COLOR_PAIR(lane->type*10 + 1));

    // Display the obstacles

    if (lane->type == WATER && lane->speed == 0) {
            attron(COLOR_PAIR(COLOR_PAIR_LILY));
        }
    else {
        attron(COLOR_PAIR(lane->type*10));
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
                    mvprintw(screen_y, i+current_obstacle->x, "0"); 
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
        attroff(COLOR_PAIR(lane->type*10));
    }

    if (lane->type == WATER) {
        attron(COLOR_PAIR(lane->type*10 + 1));
        while(coins != NULL) {
            if (round(coins->val) < LANE_WIDTH && round(coins->val) >= 0) {
                mvprintw(screen_y, (int)round(coins->val), "$");
            }
            coins = coins->next;
        }
        attroff(COLOR_PAIR(lane->type*10 + 1));
    }

}

void display(displayedData data) {
    // Display the lanes
    lane* current_lane = data.camera_first_lane;
    int lane_count = 0;
    while (lane_count <= GAME_HEIGHT) {
        display_lane(current_lane, lane_count);
        if (current_lane->y == data.player.y && data.player.skin != -1) {
            attron(COLOR_PAIR(10*current_lane->type + data.player.skin + 2));
            mvprintw(round(data.cameraY - data.player.y + 1), round(data.player.x), "*");
            attroff(COLOR_PAIR(10*current_lane->type + data.player.skin + 2));
        }
        if (current_lane->y == data.ai.y && data.ai.skin != -1) {
            attron(COLOR_PAIR(10*current_lane->type + data.ai.skin + 2));
            mvprintw(round(data.cameraY - data.ai.y + 1), round(data.ai.x), "*");
            attroff(COLOR_PAIR(10*current_lane->type + data.ai.skin + 2));
        }
        current_lane = current_lane->next;
        lane_count++;
    }

    // Display the score
    if (data.ai.skin != -1) {
        mvprintw(0, 0, "Score: %d\n", (int) data.player.y); // \n to not have issues going down from powers of 10
    }
    refresh();
}

void display_title_animation(void) {
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

void display_shop(void) {
    const char* title[] = {
        "************",
        "* THE SHOP *",
        "************"
    };

    int title_length = sizeof(title) / sizeof(title[0]);
    for (int i = 0; i < title_length; i++) {
        mvprintw(GAME_HEIGHT/4 - 3 + i, 30, "%s", title[i]);
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
    init_colors();

    int game_state = MENU;
    int high_score = 0;
    int player_color = 0;
    bool unlocked_colors[PLAYER_COLORS] = {false};
    unlocked_colors[0] = true;
    int purse = 13;
    int move_timer;
    int current_y;
    int current_x;
    bool drown_flag;
    obstacle* on_log = NULL; // pointer to the log the player is currently on, if any
    displayedData game;

    bool menu_anim = true;
    int star = 0;

    int cai = 0; // ai chosen, absent by default
    int time_ai = AI_SPEED; // timer for the ai
    obstacle* on_log_ai = NULL; // pointer to the log the ai is currently on, if any
    bool ai_dead = false;

    while (true) {
        int ch = getch(); //Get the inputs from the keyboard

        switch (game_state) {

            case MENU:
            mvprintw(0, 0, "High score: %d", high_score);

            if (menu_anim) {
                display_title_animation();
                menu_anim = false;
            } else {
                mvprintw(GAME_HEIGHT/4 - 3, (star/2)+2, "*");
                mvprintw(GAME_HEIGHT/4 - 3, (star/2)+3, " ");

                mvprintw(GAME_HEIGHT/4 + 1, 22-(star/2), "*");
                mvprintw(GAME_HEIGHT/4 + 1, 21-(star/2), " ");
                star = (star + 1) % (21*2);
            }

            mvprintw(GAME_HEIGHT/4 + 3, 3, "Press any key to play");
            mvprintw(GAME_HEIGHT/4 + 4, 2, "Press s to go to the shop");
            mvprintw(GAME_HEIGHT/4 + 5, 1, "Press a to go choose the ai");

            mvprintw(GAME_HEIGHT/4 + 7, 7, "Press q to quit");
            

            switch (ch){
                case 'q':
                endwin();
                return 0;

                case 's':
                clear();
                game_state = SHOP;
                break;
                case 'a':
                clear();
                game_state = AI;
                break;
                case ERR:
                break;
                default:
                clear();
                game_state = GAME;
                move_timer = GAME_SPEED;
                ai_dead = false;
                on_log = NULL;
                on_log_ai = NULL;
                time_ai = AI_SPEED;
                game = init_game(GAME_HEIGHT);
                if (!cai) {
                    game.ai.skin = -1;
                } else {
                    game.ai.skin = 0;
                }
                if (cai == 4) {
                    game.player.skin = -1;
                } else {
                    game.player.skin = player_color;
                }
                break;

            }
            break;

            case SHOP:
            display_shop();
            mvprintw(6, 35, "%d$", purse);
            mvprintw(0, 23, "Press m to return to menu");

            for (int i=0; i<PLAYER_COLORS; i++) {
                attron(COLOR_PAIR((LANE_TYPES+1)*10+i));
                mvprintw(GAME_HEIGHT/4 + 7, 5+(i*20), "*");
                attroff(COLOR_PAIR((LANE_TYPES+1)*10+i));

                if (unlocked_colors[i]) {
                    if (player_color == i) {
                        mvprintw(GAME_HEIGHT/4 + 9, 2+(i*20), "EQUIPPED");
                    } else {
                        mvprintw(GAME_HEIGHT/4 + 9, 1+(i*20), "%d - EQUIP", i);     
                    } 
                } else {
                    if (PRICE > purse) {
                        attron(COLOR_PAIR(RED_TEXT));
                    }
                    mvprintw(GAME_HEIGHT/4 + 9, 3+(i*20), "%d - %d$", i, PRICE);
                    attroff(COLOR_PAIR(RED_TEXT));  
                }
            }

            if (ch == 'm') {
                clear();
                menu_anim = true;
                game_state = MENU;
            }

            for (int i=0; i<PLAYER_COLORS; i++) {
                if (ch == i+'0') {
                    if (unlocked_colors[i]) {
                        player_color = i;
                        clear();
                    } else {
                        if (PRICE <= purse) {
                            purse -= PRICE;
                            unlocked_colors[i] = true;
                            clear();
                        }
                    }
                }
            }

            break;

            case AI:
            mvprintw(0, 23, "Press m to return to menu");
            // choix -> jouer seul (0), voir ia (4), jouer avec 1(1), 2(2) 
            // possiblement3 quand je l'ai fini
            mvprintw(10, 2, "0: no AI");
            mvprintw(11, 2, "1: VS. easy AI");
            mvprintw(12, 2, "2: VS. medium AI");
            mvprintw(13, 2, "4: AI alone");
            if (ch=='0' || ch=='1' || ch=='2' || ch=='4') {
                cai = ch-'0';
            }
            mvprintw(20, 2, "AI chosen: %d\n\n\nw", cai);
            if (ch == 'm') {
                clear();
                menu_anim = true;
                game_state = MENU;
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

            if (move_timer == 0 || game.cameraY - game.player.y < (GAME_HEIGHT/4) || game.cameraY - game.ai.y < (GAME_HEIGHT/4)) { 
                game = move_camera(game, 1);
                move_timer = GAME_SPEED;
            } else {
                move_timer--;
            }

            // Check if the player is colliding with something

            lane* current_lane = game.camera_first_lane;
            lane* player_lane;
            lane* ai_lane;

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

                if (current_lane->y == game.ai.y) {
                    ai_lane = current_lane;
                }

                current_lane = current_lane->next;
            }

            if (player_lane != NULL) {

                // Check for collisions with coins
                if (collides_coin(player_lane, game) != NULL) {
                    purse++;
                }

                if (player_lane->type == WATER) {
                    drown_flag = true; // set this to false to disable drowning
                    }
                else {
                    drown_flag = false;
                    }
                
                if (player_lane->type == WATER && player_lane->speed != 0) {
                    if (on_log != NULL) {
                        game.player.x += player_lane->speed;
                    }
                } else {
                    on_log = NULL;
                    game.player.x = round(game.player.x);
                    game.player.y = round(game.player.y);
                }
                

                // Check for collisions with obstacles
                obstacle* collided_obstacle = collides(player_lane, game.player);
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
                            if (on_log != collided_obstacle) {
                                on_log = collided_obstacle;
                                game.player.x = collided_obstacle->x + abs((int) round(game.player.x - collided_obstacle->x));
                            }
                        }
                        break;
                        break;
                        default:
                        break;
                    }
                }

            }

            if (game.player.y < game.cameraY - GAME_HEIGHT || drown_flag || game.player.x < UNPLAYABLE_WIDTH || game.player.x > LANE_WIDTH - UNPLAYABLE_WIDTH - 1) {
                game_state = GAME_OVER;
            }

            if (cai != 0) {
                time_ai--;
                if (time_ai == 0) {
                    game.ai = play_ai(cai, ai_lane, game.ai);
                    time_ai = AI_SPEED;
                }

                if (ai_lane != NULL) {

                if (ai_lane->type == WATER) {
                    drown_flag = true; // set this to false to disable drowning
                    }
                else {
                    drown_flag = false;
                    }
                
                if (ai_lane->type == WATER && ai_lane->speed != 0) {
                    if (on_log_ai != NULL) {
                        game.ai.x += ai_lane->speed;
                    }
                } else {
                    on_log_ai = NULL;
                    game.ai.x = round(game.ai.x);
                    game.ai.y = round(game.ai.y);
                }
                

                // Check for collisions with obstacles
                obstacle* collided_obstacle = collides(ai_lane, game.ai);
                if(collided_obstacle != NULL) {
                    switch (ai_lane->type) {
                        case GRASS:
                        game.ai.x = current_x;
                        game.ai.y = current_y;
                        break;
                        case TRACK:
                        ai_dead = true;
                        break;
                        case ROAD:
                        ai_dead = true;
                        break;
                        case WATER:
                        drown_flag = false;
                        if (player_lane->speed != 0) {
                            if (on_log_ai != collided_obstacle) {
                                on_log_ai = collided_obstacle;
                                game.ai.x = collided_obstacle->x + abs((int) round(game.ai.x - collided_obstacle->x));
                            }
                        }
                        break;
                        break;
                        default:
                        break;
                    }
                }

            }
            if (game.ai.y < game.cameraY - GAME_HEIGHT || drown_flag || game.ai.x < UNPLAYABLE_WIDTH || game.ai.x > LANE_WIDTH - UNPLAYABLE_WIDTH - 1) {
                ai_dead = true;
            }
            }

            if (game_state == GAME_OVER && cai == 4) {
                game_state = GAME;
            }

            if (game_state != GAME_OVER) {
                display(game);
                // Display the purse
                mvprintw(0, LANE_WIDTH-3, "%d$", purse);
                refresh();
            }



            if (ai_dead) {
                game_state = GAME_OVER;
            }
        
            break;

            case GAME_OVER:
            if (cai == 0) {
                mvprintw(0, 0, "Final score: %d", (int) game.player.y);
            } else if (cai == 4) {
                mvprintw(0, 0, "AI score: %d", (int) game.ai.y);
            }
            
            if (cai == 1 || cai == 2) {
                mvprintw(1, 0, "AI chosen: %d", (int)cai);
                if (ai_dead) {
                    mvprintw(GAME_HEIGHT/4, LANE_WIDTH+2, "You won!");
                } else {
                    mvprintw(GAME_HEIGHT/4, LANE_WIDTH+2, "You lost!");
                }
            } else {
                mvprintw(GAME_HEIGHT/4, LANE_WIDTH+2, "GAME OVER ;-;");
            }
            mvprintw(GAME_HEIGHT/4 + 3, LANE_WIDTH+2, "Press any key to return to menu");
            if (ch != ERR && ch != KEY_UP && ch != KEY_DOWN && ch != KEY_LEFT && ch != KEY_RIGHT) {
                clear();
                menu_anim = true;
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
