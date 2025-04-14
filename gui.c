#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>
#include "core.h"

#define WIDTH 960
#define HEIGHT 960

#define TILE_SIDE 40

#define GAME_HEIGHT (HEIGHT/TILE_SIDE)
#define GAME_SPEED 0.01 // In pixels per frame, speed of the scrolling
#define ANIM_LENGTH 10 // In frames, time it takes for the player to get to the next tile
#define PLAYER_SPEED (1.0f /ANIM_LENGTH) // In tiles per frame, speed of the player

#define PLAYER_SKINS 1 // Number of skins available

#define UP 1
#define RIGHT 90
#define DOWN 180
#define LEFT 270

enum {
    MENU,
    GAME,
    GAME_OVER,
    SHOP
};

enum {
    SKIN1 = LANE_TYPES + 1,
    TREE,
    CAR1,
    CAR2,
    LILY,
    LOG_SINGLE,
    LOG_MID,
    LOG_EDGE,
    TRAIN_MID,
    TRAIN_EDGE,
    END_TEXTURES
};

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

    textures[SKIN1] = create_texture(renderer, "sprites/skin1.png", TILE_SIDE, TILE_SIDE);
}

void display_lane(lane* lane, float lane_count, SDL_Renderer* renderer, SDL_Texture** textures) {

    float screen_y = GAME_HEIGHT - lane_count;

    for (int i = 0; i < LANE_WIDTH; i++) {
        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
        SDL_Rect destRect = {i*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
        SDL_RenderCopy(renderer, textures[lane->type], &spriteRect, &destRect);
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
    SDL_RenderCopyEx(renderer, textures[player.skin+SKIN1], &spriteRect, &destRect, player.orientation, NULL, false);
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

    displayPlayer(data.player, data.cameraY, renderer, textures);

}


int main() {

    struct timespec remaining, request = { 0, 1000000000/REFRESH_RATE}; // ~1 frame at REFRESH_RATE fps
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Crossy Road", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture** textures = (SDL_Texture**) malloc((END_TEXTURES)*(sizeof(SDL_Texture*)));
    load_textures(renderer, textures);

    SDL_Event event;
    int running = 1;
    bool action = false;


    displayedData data_test = init_game(GAME_HEIGHT);

    // Global variables
    int game_state = GAME;
    int purse = 0;
    bool drown_flag = false; // keeps track of whether the player is fully in empty waters or not
    bool blocked_path = false; // whether the path is currently blocked by a tree or not
    obstacle* on_log = NULL; // the log on which the player is
    float x_offset = 0; // in tiles per frame, speed at which x coordinate must be changed during the animation to place the player on top of a tile (specifically for going on and off logs)
    float liftboost = 0; // in tiles per frame, speed at which the player is being carried (specifically for logs)
    // int high_score = 0;
    // int player_skin = 0;
    // bool unlocked_skins[PLAYER_SKINS] = {false};
    // unlocked_skins[0] = true;

    int player_anim = 0; // timer for player animation (0 = stopped, anything else = moving)
    int buffer = 0; // stores the next movement to be performed
    bool buffer_key_flag = true; // whether the key for the last movement has been released or not


    while (running) {
        action = SDL_PollEvent(&event);

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
        case MENU:
            break;
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
    
    
            display(data_test, renderer, textures);
    
            data_test = move_camera(data_test, GAME_SPEED);
    
            lane* current_lane = data_test.camera_first_lane;
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
                if (current_lane->y == ceil(data_test.player.y)) {
                    player_top_lane = current_lane;
                } else if (current_lane->y == floor(data_test.player.y)) {
                    player_bottom_lane = current_lane;
                }
                current_lane = current_lane->next;
            }

            // Collision check
            blocked_path = false;

            if (player_top_lane != NULL) {

                // Check for collisions with coins
                for (int i = 0; i < LANE_WIDTH; i++) {
                    if (player_top_lane->coins[i] && data_test.player.x == i) {
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
                obstacle* collided_obstacle = collides(player_top_lane, data_test);

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
                            if (on_log != collided_obstacle && on_log != collided_obstacle->next && data_test.player.orientation == UP) {
                                int pos_on_log = (int) round(data_test.player.x - collided_obstacle->x); // in tiles, position of the player relative to the log
                                on_log = collided_obstacle;
                                liftboost = player_top_lane->speed;
                                x_offset = ((collided_obstacle->x + pos_on_log)-data_test.player.x)/(ANIM_LENGTH);

                                // in case the player is too far right to be on the log that's being collided with, check if there is a log right next to it
                                if (pos_on_log == collided_obstacle->size && collided_obstacle->next != NULL && floor(collided_obstacle->x + collided_obstacle->size) == floor(collided_obstacle->next->x)) {
                                    on_log = collided_obstacle->next;
                                    x_offset = (collided_obstacle->next->x - data_test.player.x)/(ANIM_LENGTH);
                               }
                            }
                        }
                        break;
                        default:
                        break;
                    }
                }

                // leaving a log
                if (on_log != NULL && !(player_top_lane->type == WATER && player_top_lane->speed != 0) && data_test.player.orientation == UP) {
                    
                    if (collided_obstacle != NULL && player_top_lane->type == GRASS) {
                        blocked_path = true;
                    } else {
                        liftboost = 0;
                        x_offset = (round(data_test.player.x) - data_test.player.x)/ANIM_LENGTH;
                        on_log = NULL;
                    }
                }
            }

            if (player_bottom_lane != NULL) {

                // Check for collisions with coins
                for (int i = 0; i < LANE_WIDTH; i++) {
                    if (player_bottom_lane->coins[i] && data_test.player.x == i) {
                        purse++;
                        player_bottom_lane->coins[i] = false;
                    }
                }

                // Check for collisions with obstacles
                obstacle* collided_obstacle = collides(player_bottom_lane, data_test);

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
                            if (on_log != collided_obstacle && on_log != collided_obstacle->next && data_test.player.orientation == DOWN) {
                                int pos_on_log = (int) round(data_test.player.x - collided_obstacle->x);
                                on_log = collided_obstacle;
                                liftboost = player_bottom_lane->speed;
                                x_offset = ((collided_obstacle->x + pos_on_log)-data_test.player.x)/(ANIM_LENGTH);

                                // in case the player is too far right to be on the log that's being collided with, check if there is a log right next to it
                                if (pos_on_log == collided_obstacle->size && collided_obstacle->next != NULL && floor(collided_obstacle->x + collided_obstacle->size) == floor(collided_obstacle->next->x)) {
                                    on_log = collided_obstacle->next;
                                    x_offset = (collided_obstacle->next->x - data_test.player.x)/(ANIM_LENGTH);
                               }
                            }
                        }
                        break;
                        default:
                        break;
                    }
                }

                if (on_log != NULL && !(player_bottom_lane->type == WATER && player_bottom_lane->speed != 0) && data_test.player.orientation == DOWN) {                
                    
                    if (collided_obstacle != NULL && player_bottom_lane->type == GRASS) {
                        blocked_path = true;
                    } else {
                        liftboost = 0;
                        x_offset = (round(data_test.player.x) - data_test.player.x)/ANIM_LENGTH;
                        on_log = NULL;
                    }
                }
            

            }

            // screen edges
            if (data_test.player.x < UNPLAYABLE_WIDTH || data_test.player.x > LANE_WIDTH - UNPLAYABLE_WIDTH - 1) {
                blocked_path = true;
            } 

            // Update player

            data_test.player.x += liftboost; // apply liftboost

            // a new action needs to be performed!
            if (!player_anim && buffer && buffer_key_flag && !blocked_path) {
                data_test.player.orientation = buffer;
                player_anim = ANIM_LENGTH;
                buffer_key_flag = false;
                buffer = 0;
            }

            // player is currently moving, update everything accordingly
            if (player_anim) {
                if (blocked_path) {
                    player_anim = 0;
                } else {
                    switch (data_test.player.orientation) {
                        case UP:
                            data_test.player.y += PLAYER_SPEED;
                            if (data_test.cameraY - data_test.player.y < (GAME_HEIGHT/4)) {
                                data_test = move_camera(data_test, PLAYER_SPEED); // move the camera if the player is too high
                            }
                            break;
                        case DOWN:
                            data_test.player.y -= PLAYER_SPEED;
                            break;
                        case RIGHT:
                            data_test.player.x += PLAYER_SPEED;
                            break;
                        case LEFT:
                            data_test.player.x -= PLAYER_SPEED;
                            break;
                    }
                    data_test.player.x += x_offset;
                    player_anim--;
                }

                // if action is ending, recenter the player and reset variables
                if (!player_anim) {
                    data_test.player.y = round(data_test.player.y); // Recenter player position to avoid float drifting
                    buffer_key_flag = true;
                    x_offset = 0;

                    if (!on_log) {
                        data_test.player.x = round(data_test.player.x);  // Recenter player position to avoid float drifting
                    }
                }

            }

            // checking for game over

            if (data_test.player.y < data_test.cameraY - GAME_HEIGHT || // player is too low
                (drown_flag && !player_anim) || // player is drowning
                (data_test.player.x < UNPLAYABLE_WIDTH && on_log != NULL) || // player is being carried offscreen
                (data_test.player.x > LANE_WIDTH - UNPLAYABLE_WIDTH - 1 && on_log != NULL)) // player is being carreid offscreen
            {
                game_state = GAME_OVER;
            }

            if (!player_anim && on_log != NULL) {
                if (data_test.player.x > on_log->x + on_log->size - 0.1) { // check if player is too far right on the log it's currently on
                    if (on_log->next != NULL && floor(on_log->x + on_log->size) == floor(on_log->next->x)) { // check if player can move right to a different adjacent log
                        on_log = on_log->next;
                    } else {
                        game_state = GAME_OVER; // if not, game over
                    }
                }
                else if (data_test.player.x+0.1 < on_log->x) { // check if player is too far left on the log it's currently on
                    if (on_log->prev != NULL && floor(on_log->prev->x + on_log->prev->size) == floor(on_log->x)) { // check if player can move left to a different adjacent log
                        on_log = on_log->prev;
                    } else {
                        game_state = GAME_OVER; // if not, game over
                    }
                }
            }
    
            break;
        case GAME_OVER:
            break;
        default:
            break;
        }

        SDL_RenderPresent(renderer);
        nanosleep(&request, &remaining); 
    }


    for (int i=0; i<END_TEXTURES-1; i++) {
        SDL_DestroyTexture(textures[i+1]);
    }

    free_lanes(data_test.first_lane);
    free(textures);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
