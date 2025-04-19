#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <time.h>
#include "core.h"
#include "gui.h"

#define WIDTH 960
#define HEIGHT 960

#define TILE_SIDE 40

#define GAME_HEIGHT (HEIGHT/TILE_SIDE)
#define GAME_SPEED 0.01 // In pixels per frame, speed of the scrolling
#define ANIM_LENGTH 7 // In frames, time it takes for the player to get to the next tile
#define PLAYER_SPEED (1.0f /ANIM_LENGTH) // In tiles per frame, speed of the player

#define UP 1
#define RIGHT 90
#define DOWN 180
#define LEFT 270

enum {
    MENU,
    MENU_TO_GAME,
    GAME,
    GAME_OVER,
    GO_TO_MENU,
    GAMBLING,
    GAMBLED,
    SKIN_SELECT,
};

enum {
    TREE = LANE_TYPES + 1,
    CAR1,
    CAR2,
    LILY,
    LOG_SINGLE,
    LOG_MID,
    LOG_EDGE,
    TRAIN_MID,
    TRAIN_EDGE,
    WARNING,
    COIN,
    PLAY_BUTTON,
    SKINS_BUTTON,
    MENU_BUTTON,
    GAMBLE_BUTTON,
    TITLE_CARD,
    SKIN_BG,
    LOCK,
    GAME_OVER_CARD,
    SKIN_START,
    END_TEXTURES
};

#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 100

#define CARD_WIDTH 400
#define CARD_HEIGHT 200

#define SKIN_SIDE 100
#define SKINS 6
#define SKINS_PER_LINE (int) ((WIDTH-SKIN_SIDE*2)/(SKIN_SIDE*1.5)) // how many skins to be displayed by line
#define PRICE 5

#define FADE_LENGTH 50 // in frames, duration of the transitions
#define GAMBLING_DURATION 50 // in frames, duration of the gambling animation

#define REFRESH_RATE 60

SDL_Texture* create_texture(SDL_Renderer* renderer, char* filename, int width, int height) {
    SDL_Surface *surface = IMG_Load(filename);
    SDL_Surface *resizedSurface = SDL_CreateRGBSurface(0, width, height, surface->format->BitsPerPixel,
        surface->format->Rmask, surface->format->Gmask,
        surface->format->Bmask, surface->format->Amask);

    SDL_BlitScaled(surface, NULL, resizedSurface, NULL);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, resizedSurface);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(resizedSurface);

    return texture;
}

void load_textures(SDL_Renderer* renderer, SDL_Texture** textures) {
    textures[GRASS] = create_texture(renderer, "sprites/grass.png", TILE_SIDE, TILE_SIDE);
    textures[WATER] = create_texture(renderer, "sprites/water.png", TILE_SIDE, TILE_SIDE);
    textures[ROAD] = create_texture(renderer, "sprites/road.png", TILE_SIDE, TILE_SIDE);   
    textures[TRACK] = create_texture(renderer, "sprites/track.png", TILE_SIDE, TILE_SIDE);

    textures[TREE] = create_texture(renderer, "sprites/tree.png", TILE_SIDE, TILE_SIDE);
    textures[CAR1] = create_texture(renderer, "sprites/car1.png", TILE_SIDE, TILE_SIDE);
    textures[CAR2] = create_texture(renderer, "sprites/car2.png", TILE_SIDE*2, TILE_SIDE);
    textures[LILY] = create_texture(renderer, "sprites/lily.png", TILE_SIDE, TILE_SIDE);
    textures[LOG_SINGLE] = create_texture(renderer, "sprites/log_single.png", TILE_SIDE, TILE_SIDE);
    textures[LOG_EDGE] = create_texture(renderer, "sprites/log_edge.png", TILE_SIDE, TILE_SIDE);
    textures[LOG_MID] = create_texture(renderer, "sprites/log_mid.png", TILE_SIDE, TILE_SIDE);
    textures[TRAIN_EDGE] = create_texture(renderer, "sprites/train_edge.png", TILE_SIDE, TILE_SIDE);
    textures[TRAIN_MID] = create_texture(renderer, "sprites/train_mid.png", TILE_SIDE, TILE_SIDE);

    textures[WARNING] = create_texture(renderer, "sprites/warning.png", TILE_SIDE, TILE_SIDE);
    textures[COIN] = create_texture(renderer, "sprites/coin.png", TILE_SIDE, TILE_SIDE);


    textures[PLAY_BUTTON] = create_texture(renderer, "sprites/text/play_button.png", BUTTON_WIDTH, BUTTON_HEIGHT);
    textures[MENU_BUTTON] = create_texture(renderer, "sprites/text/menu_button.png", BUTTON_WIDTH, BUTTON_HEIGHT);
    textures[SKINS_BUTTON] = create_texture(renderer, "sprites/text/skins_button.png", BUTTON_WIDTH, BUTTON_HEIGHT);
    textures[GAMBLE_BUTTON] = create_texture(renderer, "sprites/text/gamble_button.png", BUTTON_WIDTH, BUTTON_HEIGHT);

    textures[LOCK] = create_texture(renderer, "sprites/lock.png", SKIN_SIDE, SKIN_SIDE);

    textures[TITLE_CARD] = create_texture(renderer, "sprites/text/title.png", CARD_WIDTH, CARD_HEIGHT);
    textures[GAME_OVER_CARD] = create_texture(renderer, "sprites/text/game_over.png", CARD_WIDTH, CARD_HEIGHT);

    textures[SKIN_BG] = create_texture(renderer, "sprites/text/skin_bg.png", SKIN_SIDE, SKIN_SIDE);

    for (int i=0; i<SKINS; i++) {
        char s[24];
        sprintf(s, "sprites/skins/skin%d.png", i);
        textures[SKIN_START+i] = create_texture(renderer, s, TILE_SIDE, TILE_SIDE);
    }
}



void display_text(char* text, int x, int y, int size, SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("fonts/arial.ttf", size);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}


void display_lane(lane* lane, float lane_count, SDL_Renderer* renderer, SDL_Texture** textures) {

    float screen_y = GAME_HEIGHT - lane_count;

    for (int i = 0; i < LANE_WIDTH; i++) {
        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
        SDL_Rect destRect = {i*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
        SDL_RenderCopy(renderer, textures[lane->type], &spriteRect, &destRect);
    }

    // Display coins
    for (int i = 0; i < LANE_WIDTH; i++) {
        if (lane->coins[i]) {
            SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
            SDL_Rect destRect = {i*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
            SDL_RenderCopy(renderer, textures[COIN], &spriteRect, &destRect);
        }
    }

    // Display obstacles
    obstacle* current_obstacle = lane->obstacles;
    int flip = (lane->speed < 0);

    if (current_obstacle != NULL) {
        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
    
        while (current_obstacle != NULL) {
            SDL_Rect destRect = {current_obstacle->x*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};

            switch (lane->type)
            {
            case GRASS:
                SDL_RenderCopy(renderer, textures[TREE], &spriteRect, &destRect);
                break;
            case WATER:
                if (lane->speed == 0) {
                    SDL_RenderCopy(renderer, textures[LILY], &spriteRect, &destRect);
                    } else {
                        if (current_obstacle->size > 1) {
                            SDL_RenderCopyEx(renderer, textures[LOG_EDGE], &spriteRect, &destRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                            for (int i=1; i<(current_obstacle->size-1); i++) {
                                SDL_Rect destRect2 = {(current_obstacle->x+i)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                                SDL_RenderCopy(renderer, textures[LOG_MID], &spriteRect, &destRect2);
                            }
                            SDL_Rect destRect3 = {(current_obstacle->x+current_obstacle->size-1)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                            SDL_RenderCopy(renderer, textures[LOG_EDGE], &spriteRect, &destRect3);
                        } else {
                            SDL_RenderCopy(renderer, textures[LOG_SINGLE], &spriteRect, &destRect);
                        }
                }   
                break;
            case TRACK:
                SDL_RenderCopyEx(renderer, textures[TRAIN_EDGE], &spriteRect, &destRect, 0.0, NULL, flip);
                for (int i=1; i<(current_obstacle->size); i++) {
                    SDL_Rect destRect2 = {(current_obstacle->x+i)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                    SDL_RenderCopy(renderer, textures[TRAIN_MID], &spriteRect, &destRect2);
                }
                SDL_Rect destRect3 = {(current_obstacle->x+current_obstacle->size-1)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                SDL_RenderCopy(renderer, textures[TRAIN_EDGE], &spriteRect, &destRect3);

                if (!flip) {
                    if (current_obstacle->x + current_obstacle->size > -WARNING_TIME*TRAIN_SPEED && current_obstacle->x + current_obstacle->size < 0) {
                        SDL_Rect warningRect = {0, 0, TILE_SIDE, TILE_SIDE};
                        SDL_Rect destRect2 = {0, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                        SDL_RenderCopy(renderer, textures[WARNING], &warningRect, &destRect2);
                    }
                } else {
                    if (current_obstacle->x < LANE_WIDTH + WARNING_TIME*TRAIN_SPEED && current_obstacle->x > LANE_WIDTH) {
                        SDL_Rect warningRect = {0, 0, TILE_SIDE, TILE_SIDE};
                        SDL_Rect destRect2 = {(LANE_WIDTH-1)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                        SDL_RenderCopy(renderer, textures[WARNING], &warningRect, &destRect2);
                    }
                }
                break;
            case ROAD:
                if (current_obstacle->size == 1) {
                    SDL_RenderCopyEx(renderer, textures[CAR1], &spriteRect, &destRect, 0.0, NULL, flip);
                } else {
                    SDL_Rect spriteRect2 = {0, 0, TILE_SIDE*2, TILE_SIDE};
                    SDL_Rect destRect2 = {current_obstacle->x*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE*2, TILE_SIDE};
                    SDL_RenderCopyEx(renderer, textures[CAR2], &spriteRect2, &destRect2, 0.0, NULL, flip);
                }
                break;  
            default:
                break;
            }
        current_obstacle = current_obstacle->next;
        }
    }
}

void displayPlayer(player player, float cameraY, SDL_Renderer* renderer, SDL_Texture** textures) {
    SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
    SDL_Rect destRect = {player.x*TILE_SIDE, (cameraY- player.y)*TILE_SIDE, TILE_SIDE, TILE_SIDE};
    SDL_RenderCopyEx(renderer, textures[player.skin+SKIN_START], &spriteRect, &destRect, player.orientation, NULL, false);
}


void display(displayedData data, SDL_Renderer* renderer, SDL_Texture** textures) {
    // Display the lanes
    lane* current_lane = data.camera_first_lane;
    
    float lane_count = (int)(data.cameraY) - data.cameraY;
    while (lane_count <= GAME_HEIGHT+1) {
        display_lane(current_lane, lane_count, renderer, textures);
        current_lane = current_lane->next;
        lane_count++;
    }

    if (data.player.skin != -1) {
        displayPlayer(data.player, data.cameraY, renderer, textures);

        // Display score
        char scoreText[20];
        sprintf(scoreText, "Score: %d", (int) data.player.y);
        display_text(scoreText, 0, 0, 24, renderer);
    }
}

void display_button(button b, SDL_Renderer* renderer, SDL_Texture** textures) {
    SDL_Rect spriteRect = {0, 0, BUTTON_WIDTH, BUTTON_HEIGHT};
    SDL_Rect destRect = {b.x, b.y, b.width, b.height};
    SDL_RenderCopy(renderer, textures[b.texture], &spriteRect, &destRect);
}

bool button_clicked(button b, SDL_Event event) {
    return event.button.x > b.x
        && event.button.x <= b.x + b.width
        && event.button.y > b.y
        && event.button.y <= b.y + b.height;
}

int main() {

    struct timespec remaining, request = { 0, 1000000000/REFRESH_RATE}; // ~1 frame at REFRESH_RATE fps
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Crossy Road", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture** textures = (SDL_Texture**) malloc((END_TEXTURES+SKINS)*(sizeof(SDL_Texture*)));
    load_textures(renderer, textures);

    SDL_Event event;
    int running = 1;
    bool action = false;


    

    int game_state = MENU;
    int player_skin = 0;
    int purse = 0;
    bool unlocked_skins[SKINS] = {true};
    for (int i=1; i<SKINS; i++) {
        unlocked_skins[i] = false;
    }
    unlocked_skins[0] = true;

    // Game-specific variables
    displayedData game = {0, NULL, NULL, {0, 0, 0, 0}, 0, 0};

    bool drown_flag = false; // keeps track of whether the player is fully in empty waters or not
    bool blocked_path = false; // whether the path is currently blocked by a tree or not
    obstacle* on_log = NULL; // the log on which the player is
    float x_offset = 0; // in tiles per frame, speed at which x coordinate must be changed during the animation to place the player on top of a tile (specifically for going on and off logs)
    float liftboost = 0; // in tiles per frame, speed at which the player is being carried (specifically for logs)
    int high_score = 0;

    int player_anim = 0; // timer for player animation (0 = stopped, anything else = moving)
    int buffer = 0; // stores the next movement to be performed
    bool buffer_key_flag = true; // whether the key for the last movement has been released or not
    
    int skin_to_unlock = 0;
    // Menu and transition variables
    displayedData demo = init_game(GAME_HEIGHT);
    demo.player.skin = -1;
    int fade = FADE_LENGTH;

    // Buttons

    button skin_buttons[SKINS] = {{0, 0, SKIN_SIDE, SKIN_SIDE, SKIN_BG}};

    for (int i=0; i<SKINS; i++) {
        skin_buttons[i].x = ((i%SKINS_PER_LINE)*SKIN_SIDE*1.5)+SKIN_SIDE;
        skin_buttons[i].y = (i/SKINS_PER_LINE*SKIN_SIDE*1.5)+SKIN_SIDE;
        skin_buttons[i].width = SKIN_SIDE;
        skin_buttons[i].height = SKIN_SIDE;
        skin_buttons[i].texture = SKIN_BG;
    }

    while (running) {
        action = SDL_PollEvent(&event);
        SDL_FlushEvent(SDL_MOUSEMOTION);

        if (action) {
            switch (event.type) {
            case SDL_QUIT:
                running = 0;
                break;
            default:
                break;
            }
        }

        switch(game_state) {
        case MENU: {
            // Demo background
            SDL_RenderClear(renderer);
            display(demo, renderer, textures);
    
            demo = move_camera(demo, GAME_SPEED*5);
    
            lane* current_lane = demo.camera_first_lane;

            while (current_lane->next != NULL) {
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

                current_lane = current_lane->next;
            }

            // Display buttons

            button play = {(WIDTH-BUTTON_WIDTH)/2, (HEIGHT-BUTTON_HEIGHT)/2, BUTTON_WIDTH, BUTTON_HEIGHT, PLAY_BUTTON};
            button skins = {(WIDTH-BUTTON_WIDTH)/2, (HEIGHT-BUTTON_HEIGHT)/2 + 1.5*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, SKINS_BUTTON};
            button gamble = {(WIDTH-BUTTON_WIDTH)/2, (HEIGHT-BUTTON_HEIGHT)/2 + 3*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, GAMBLE_BUTTON};

            display_button(play, renderer, textures);
            display_button(skins, renderer, textures);
            if (purse >= PRICE) {
                display_button(gamble, renderer, textures);
            } else {
                SDL_Rect spriteRect = {0, 0, BUTTON_WIDTH, BUTTON_HEIGHT};
                SDL_Rect destRect = {gamble.x, gamble.y, BUTTON_WIDTH, BUTTON_HEIGHT};
                SDL_RenderCopy(renderer, textures[LOCK], &spriteRect, &destRect);
            }

            switch (event.type) {
                case SDL_MOUSEBUTTONUP:
                    if (button_clicked(play, event)) {
                            game_state = MENU_TO_GAME;
                        }
                    else if (button_clicked(skins, event)) {
                        game_state = SKIN_SELECT;
                    }
                    else if (button_clicked(gamble, event) && purse >= PRICE) {
                        purse -= PRICE;
                        fade = GAMBLING_DURATION;
                        game_state = GAMBLING;
                        skin_to_unlock = (rand() % (SKINS-1))+1;
                    }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        game_state = MENU_TO_GAME;
                    }
            }

            SDL_Rect spriteRect2 = {0, 0, CARD_WIDTH, CARD_HEIGHT};
            SDL_Rect destRect2 = {(WIDTH-CARD_WIDTH)/2, (HEIGHT/2-CARD_HEIGHT)/2, CARD_WIDTH, CARD_HEIGHT};
            SDL_RenderCopy(renderer, textures[TITLE_CARD], &spriteRect2, &destRect2);

            // Display high score
            char highScoreText[20];
            sprintf(highScoreText, "High Score: %d", high_score);
            display_text(highScoreText, 0, 0, 24, renderer);
            // Display purse
            char purseText[20];
            sprintf(purseText, "%d$", purse);
            display_text(purseText, WIDTH-50, 0, 24, renderer);

            break;
        }

        case MENU_TO_GAME: {
            if (fade) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

                if (fade >= FADE_LENGTH/2) {
                    SDL_Rect bg = {0, 0, WIDTH, (int) ((float) (FADE_LENGTH-fade)/FADE_LENGTH*HEIGHT*2)};
                    SDL_RenderFillRect(renderer, &bg);

                    if (fade == FADE_LENGTH/2) {
                        free_lanes(game.first_lane);
                        game = init_game(GAME_HEIGHT);
                        on_log = NULL;
                        x_offset = 0;
                        liftboost = 0;
                        player_anim = 0;
                        game.player.skin = player_skin;
                        buffer = 0;
                        buffer_key_flag = true;
                    }
                } else {
                    display(game, renderer, textures);
                    SDL_Rect bg = {0, 0, WIDTH, (int) ((float) (fade)/FADE_LENGTH*HEIGHT*2)};
                    SDL_RenderFillRect(renderer, &bg);
                }
                fade--;
            } else {
                game_state = GAME;
                fade = FADE_LENGTH;
            }
            break;
        }

        case GAME:
            if (action) {
                switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                    case SDLK_DOWN:
                        buffer = DOWN;
                        break;
                    case SDLK_UP:
                        buffer = UP;
                        break;
                    case SDLK_LEFT:
                        buffer = LEFT;
                        break;
                    case SDLK_RIGHT:
                        buffer = RIGHT;
                        break;
                    }
                    break;
                case SDL_KEYUP:
                    buffer_key_flag = true;
                    break;
                }
            }
    
    
            display(game, renderer, textures);
            // Display the purse
            char purseText[20];
            sprintf(purseText, "%d$", purse);
            display_text(purseText, WIDTH-24*3, 0, 24, renderer);
    
            game = move_camera(game, GAME_SPEED);
    
            lane* current_lane = game.camera_first_lane;
            lane* player_top_lane = NULL; // lane on or above the player
            lane* player_bottom_lane = NULL; // lane below the player

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
                // find the one or two lanes the player is colliding with
                if (current_lane->y == ceil(game.player.y)) {
                    player_top_lane = current_lane;
                } else if (current_lane->y == floor(game.player.y)) {
                    player_bottom_lane = current_lane;
                }
                current_lane = current_lane->next;
            }

            // Collision check
            blocked_path = false;

            if (player_top_lane != NULL) {

                // Check for collisions with coins
                for (int i = 0; i < LANE_WIDTH; i++) {
                    if (player_top_lane->coins[i] && game.player.x == i) {
                        purse++;
                        player_top_lane->coins[i] = false;
                    }
                }

                if (player_top_lane->type == WATER) {
                    drown_flag = true; // only matters for the top lane, which is the lane the player is on or is going to
                    }
                else {
                    drown_flag = false;
                    }

                //Check for collisions with obstacles
                obstacle* collided_obstacle = collides(player_top_lane, game);

                if(collided_obstacle != NULL) {
                    switch (player_top_lane->type) {
                        case GRASS:
                        if (on_log == NULL) { // case where the player is on a log is handled lower
                            blocked_path = true;
                        }
                        break;
                        case TRACK:
                        game_state = GAME_OVER;
                        break;
                        case ROAD:
                        game_state = GAME_OVER;
                        break;
                        case WATER:
                        drown_flag = false; // an obstacle has been found, player now should not drown
                        if (player_top_lane->speed != 0) { // if logs are on the lane

                            // case where we are going up to different log than before or from ground
                            if (on_log != collided_obstacle && on_log != collided_obstacle->next && game.player.orientation == UP) {
                                int pos_on_log = (int) round(game.player.x - collided_obstacle->x); // in tiles, position of the player relative to the log
                                on_log = collided_obstacle;
                                liftboost = player_top_lane->speed;
                                x_offset = ((collided_obstacle->x + pos_on_log)-game.player.x)/(ANIM_LENGTH);

                                // in case the player is too far right to be on the log that's being collided with, check if there is a log right next to it
                                if (pos_on_log == collided_obstacle->size && collided_obstacle->next != NULL && floor(collided_obstacle->x + collided_obstacle->size) == floor(collided_obstacle->next->x)) {
                                    on_log = collided_obstacle->next;
                                    x_offset = (collided_obstacle->next->x - game.player.x)/(ANIM_LENGTH);
                               }
                            }
                        }
                        break;
                        default:
                        break;
                    }
                }

                // leaving a log
                if (on_log != NULL && !(player_top_lane->type == WATER && player_top_lane->speed != 0) && game.player.orientation == UP) {
                    
                    if (collided_obstacle != NULL && player_top_lane->type == GRASS) {
                        blocked_path = true;
                    } else {
                        liftboost = 0;
                        x_offset = (round(game.player.x) - game.player.x)/ANIM_LENGTH;
                        on_log = NULL;
                    }
                }
            }

            if (player_bottom_lane != NULL) {

                // Check for collisions with coins
                for (int i = 0; i < LANE_WIDTH; i++) {
                    if (player_bottom_lane->coins[i] && game.player.x == i) {
                        purse++;
                        player_bottom_lane->coins[i] = false;
                    }
                }

                // Check for collisions with obstacles
                obstacle* collided_obstacle = collides(player_bottom_lane, game);

                if(collided_obstacle != NULL) {
                    switch (player_bottom_lane->type) {
                        case GRASS:
                        if (x_offset == 0) {
                            blocked_path = true;
                        }
                        break;
                        case TRACK:
                        game_state = GAME_OVER;
                        break;
                        case ROAD:
                        game_state = GAME_OVER;
                        break;
                        case WATER:
                        if (player_bottom_lane->speed != 0) {

                            // case where we are going down to different log than before or from ground
                            if (on_log != collided_obstacle && on_log != collided_obstacle->next && game.player.orientation == DOWN) {
                                int pos_on_log = (int) round(game.player.x - collided_obstacle->x);
                                on_log = collided_obstacle;
                                liftboost = player_bottom_lane->speed;
                                x_offset = ((collided_obstacle->x + pos_on_log)-game.player.x)/(ANIM_LENGTH);

                                // in case the player is too far right to be on the log that's being collided with, check if there is a log right next to it
                                if (pos_on_log == collided_obstacle->size && collided_obstacle->next != NULL && floor(collided_obstacle->x + collided_obstacle->size) == floor(collided_obstacle->next->x)) {
                                    on_log = collided_obstacle->next;
                                    x_offset = (collided_obstacle->next->x - game.player.x)/(ANIM_LENGTH);
                               }
                            }
                        }
                        break;
                        default:
                        break;
                    }
                }

                if (on_log != NULL && !(player_bottom_lane->type == WATER && player_bottom_lane->speed != 0) && game.player.orientation == DOWN) {                
                    
                    if (collided_obstacle != NULL && player_bottom_lane->type == GRASS) {
                        blocked_path = true;
                    } else {
                        liftboost = 0;
                        x_offset = (round(game.player.x) - game.player.x)/ANIM_LENGTH;
                        on_log = NULL;
                    }
                }
            

            }

            // screen edges
            if (game.player.x < UNPLAYABLE_WIDTH || game.player.x > LANE_WIDTH - UNPLAYABLE_WIDTH - 1) {
                blocked_path = true;
            } 

            // Update player

            game.player.x += liftboost; // apply liftboost

            // a new action needs to be performed!
            if (!player_anim && buffer && buffer_key_flag && !blocked_path && !drown_flag) {
                game.player.orientation = buffer;
                player_anim = ANIM_LENGTH;
                buffer_key_flag = false;
                buffer = 0;
            }

            // player is currently moving, update everything accordingly
            if (player_anim) {
                if (blocked_path) {
                    player_anim = 0;
                } else {
                    switch (game.player.orientation) {
                        case UP:
                            game.player.y += PLAYER_SPEED;
                            if (game.cameraY - game.player.y < (GAME_HEIGHT/4)) {
                                game = move_camera(game, PLAYER_SPEED); // move the camera if the player is too high
                            }
                            break;
                        case DOWN:
                            game.player.y -= PLAYER_SPEED;
                            break;
                        case RIGHT:
                            game.player.x += PLAYER_SPEED;
                            break;
                        case LEFT:
                            game.player.x -= PLAYER_SPEED;
                            break;
                    }
                    game.player.x += x_offset;
                    player_anim--;
                }

                // if action is ending, recenter the player and reset variables
                if (!player_anim) {
                    game.player.y = round(game.player.y); // Recenter player position to avoid float drifting
                    buffer_key_flag = true;
                    x_offset = 0;

                    if (!on_log) {
                        game.player.x = round(game.player.x);  // Recenter player position to avoid float drifting
                    }
                }

            }

            // checking for game over

            if (game.player.y < game.cameraY - GAME_HEIGHT || // player is too low
                (drown_flag && !player_anim) || // player is drowning
                (game.player.x < UNPLAYABLE_WIDTH && on_log != NULL) || // player is being carried offscreen
                (game.player.x > LANE_WIDTH - UNPLAYABLE_WIDTH - 1 && on_log != NULL)) // player is being carreid offscreen
            {
                game_state = GAME_OVER;
            }

            if (!player_anim && on_log != NULL) {
                if (game.player.x > on_log->x + on_log->size - 0.1) { // check if player is too far right on the log it's currently on
                    if (on_log->next != NULL && floor(on_log->x + on_log->size) == floor(on_log->next->x)) { // check if player can move right to a different adjacent log
                        on_log = on_log->next;
                    } else {
                        game_state = GAME_OVER; // if not, game over
                    }
                }
                else if (game.player.x+0.1 < on_log->x) { // check if player is too far left on the log it's currently on
                    if (on_log->prev != NULL && floor(on_log->prev->x + on_log->prev->size) == floor(on_log->x)) { // check if player can move left to a different adjacent log
                        on_log = on_log->prev;
                    } else {
                        game_state = GAME_OVER; // if not, game over
                    }
                }
            }
            break;
        case GAME_OVER: {
            button menu_button = {(WIDTH-BUTTON_WIDTH)/2, (HEIGHT-BUTTON_HEIGHT)/2, BUTTON_WIDTH, BUTTON_HEIGHT, MENU_BUTTON};
            display_button(menu_button, renderer, textures);

            switch (event.type) {
                case SDL_MOUSEBUTTONUP:
                    if (button_clicked(menu_button, event)) {
                            game_state = GO_TO_MENU;
                        }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        game_state = GO_TO_MENU;
                    }
            }
            SDL_Rect spriteRect2 = {0, 0, CARD_WIDTH, CARD_HEIGHT};
            SDL_Rect destRect2 = {(WIDTH-CARD_WIDTH)/2, (HEIGHT/2-CARD_HEIGHT)/2, CARD_WIDTH, CARD_HEIGHT};
            SDL_RenderCopy(renderer, textures[GAME_OVER_CARD], &spriteRect2, &destRect2);
            break;
        }
        case GO_TO_MENU: {
            if (fade) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

                if (fade >= FADE_LENGTH/2) {
                    SDL_Rect bg = {0, 0, WIDTH, (int) ((float) (FADE_LENGTH-fade)/FADE_LENGTH*HEIGHT*2)};
                    SDL_RenderFillRect(renderer, &bg);
                } else {
                    display(demo, renderer, textures);
                    SDL_Rect bg = {0, 0, WIDTH, (int) ((float) (fade)/FADE_LENGTH*HEIGHT*2)};
                    SDL_RenderFillRect(renderer, &bg);
                }
                fade--;
            } else {
                if (high_score < (int) game.player.y) {
                    high_score = (int) game.player.y;
                }
                game_state = MENU;
                fade = FADE_LENGTH;
            }
            break;
        }
        case SKIN_SELECT: {
            SDL_RenderClear(renderer);
            display(demo, renderer, textures);
            demo = move_camera(demo, GAME_SPEED*5);
            lane* current_lane = demo.camera_first_lane;
            while (current_lane->next != NULL) {
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
                current_lane = current_lane->next;
            }

            button menu_button = {0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, MENU_BUTTON};
            display_button(menu_button, renderer, textures);

            for (int i=0; i<SKINS; i++) {
                if (player_skin == i) {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
                    SDL_Rect bg = {skin_buttons[i].x, skin_buttons[i].y, SKIN_SIDE, SKIN_SIDE};
                    SDL_RenderFillRect(renderer, &bg);
                }

                display_button(skin_buttons[i], renderer, textures);
                SDL_Rect spriteRect = {0, 0, SKIN_SIDE, SKIN_SIDE};
                SDL_Rect destRect = {skin_buttons[i].x, skin_buttons[i].y, SKIN_SIDE, SKIN_SIDE};
                SDL_RenderCopy(renderer, textures[SKIN_START+i], &spriteRect, &destRect);

                if (!unlocked_skins[i]) {
                    SDL_Rect lockRect = {skin_buttons[i].x, skin_buttons[i].y, SKIN_SIDE, SKIN_SIDE};
                    SDL_RenderCopy(renderer, textures[LOCK], &spriteRect, &lockRect);
                }
            }
            switch (event.type) {
                case SDL_MOUSEBUTTONUP:
                    if (button_clicked(menu_button, event)) {
                            game_state = MENU;
                        } else {     
                        for (int i=0; i<SKINS; i++) {
                            if (button_clicked(skin_buttons[i], event)) {
                                if (unlocked_skins[i]) {
                                    player_skin = i;
                                }
                            }
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        game_state = MENU;
                    }
            }
            break;
        }
        case GAMBLING: {
            SDL_RenderClear(renderer);
            display(demo, renderer, textures);
            demo = move_camera(demo, GAME_SPEED*5);
            lane* current_lane = demo.camera_first_lane;
            while (current_lane->next != NULL) {
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
                current_lane = current_lane->next;
            }
            
            // Display unlocked skin

            int icon_size = SKIN_SIDE*4*((float) GAMBLING_DURATION-fade)/GAMBLING_DURATION;

            SDL_Rect spriteRect = {0, 0, icon_size, icon_size};
            SDL_Rect destRect = {WIDTH/2-icon_size/2, HEIGHT/2-icon_size/2, icon_size, icon_size};
            SDL_RenderCopy(renderer, textures[SKIN_START+skin_to_unlock], &spriteRect, &destRect);

            fade--;
            if (!fade) {
                fade = GAMBLING_DURATION;
                game_state = GAMBLED;
                if (!unlocked_skins[skin_to_unlock]) {
                    display_text("New skin unlocked!", WIDTH/2-200, HEIGHT/2+icon_size/2, 50, renderer);
                } else {
                    display_text("You already have this skin!", WIDTH/2-300, HEIGHT/2+icon_size/2, 50, renderer);
                }
                unlocked_skins[skin_to_unlock] = true;
            }
            break;
        }
        case GAMBLED: {

            button menu_button = {0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, MENU_BUTTON};
            display_button(menu_button, renderer, textures);

            switch (event.type) {
                case SDL_MOUSEBUTTONUP:
                    if (button_clicked(menu_button, event)) {
                            game_state = MENU;
                        }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        game_state = MENU;
                    }
            }
            break;
        }
        default:
            break;
        }

        SDL_RenderPresent(renderer);
        nanosleep(&request, &remaining); 
    }


    for (int i=0; i<END_TEXTURES+SKINS-2; i++) {
        SDL_DestroyTexture(textures[i+1]);
    }
    
    free_lanes(game.first_lane);
    free_lanes(demo.first_lane);
    free(textures);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
