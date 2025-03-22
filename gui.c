#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>
#include "core.h"

#define WIDTH 480
#define HEIGHT 480

#define TILE_SIDE 20
#define GAME_HEIGHT (WIDTH/TILE_SIDE)
#define GAME_SPEED 0.03 // In pixels per frame, speed of the scrolling

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

void load_textures(SDL_Renderer* renderer, SDL_Texture** lane_textures) {
    lane_textures[GRASS] = create_texture(renderer, "sprites/grass.png", TILE_SIDE, TILE_SIDE);
    lane_textures[WATER] = create_texture(renderer, "sprites/water.png", TILE_SIDE, TILE_SIDE);
    lane_textures[ROAD] = create_texture(renderer, "sprites/road.png", TILE_SIDE, TILE_SIDE);   
    lane_textures[TRACK] = create_texture(renderer, "sprites/track.png", TILE_SIDE, TILE_SIDE);
}

void display_lane(lane* lane, float lane_count, SDL_Renderer* renderer, SDL_Texture** lane_textures) {

    float screen_y = GAME_HEIGHT - lane_count;

    for (int i = 0; i < LANE_WIDTH; i++) {
        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
        SDL_Rect destRect = {i*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
        SDL_RenderCopy(renderer, lane_textures[lane->type], &spriteRect, &destRect);
    }
}

void display(displayedData data, SDL_Renderer* renderer, SDL_Texture** lane_textures) {
    // Display the lanes
    lane* current_lane = data.camera_first_lane;
    float lane_count = (int)(data.cameraY) - data.cameraY;
    while (lane_count <= GAME_HEIGHT+1) {
        display_lane(current_lane, lane_count, renderer, lane_textures);
        current_lane = current_lane->next;
        lane_count++;
    }
}


int main() {

    struct timespec remaining, request = { 0, 1000000000/REFRESH_RATE}; // ~1 frame at REFRESH_RATE fps

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Crossy Road", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture** lane_textures = (SDL_Texture**) malloc((LANE_TYPES+1)*(sizeof(SDL_Texture*)));
    load_textures(renderer, lane_textures);

    SDL_Event event;
    int running = 1;


    displayedData data_test = init_game(GAME_HEIGHT);

    while (running) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_DOWN:
                    break;
                }
                break;
            case SDL_KEYUP:
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        display(data_test, renderer, lane_textures);

        data_test = move_camera(data_test, GAME_SPEED);

        SDL_RenderPresent(renderer);
        
        nanosleep(&request, &remaining); 
    }


    for (int i=0; i<LANE_TYPES; i++) {
        SDL_DestroyTexture(lane_textures[i+1]);
    }

    free_lanes(data_test.first_lane);
    free(lane_textures);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
