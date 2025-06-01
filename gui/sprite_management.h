#pragma once

#include "../core.h"
#include "button.h"
#include <stdio.h>
#include <SDL2/SDL.h>

#define WIDTH 960
#define HEIGHT 960

#define TILE_SIDE 40

#define SKIN_SIDE 150
#define SKINS 10

#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 100

#define CARD_WIDTH 400
#define CARD_HEIGHT 200

#define GAME_HEIGHT (HEIGHT/TILE_SIDE)

#define POWER_START (SKIN_START + SKINS - 1)
#define END_TEXTURES (POWER_START + POWERS_END)

enum {
    TREE = LANE_TYPES + 1,
    CAR1,
    CAR2,
    LILY,
    DUCK,
    LOG_SINGLE,
    LOG_MID,
    LOG_EDGE,
    TRAIN_MID,
    TRAIN_EDGE,
    WARNING,
    CROSS,
    COIN,
    CLOUD,
    SOUND_OFF,
    SOUND_ON,
    PLAY_BUTTON,
    SKINS_BUTTON,
    MENU_BUTTON,
    GAMBLE_BUTTON,
    TITLE_CARD,
    SKIN_BG,
    LOCK,
    GAME_OVER_CARD,
    SKIN_START,
};

SDL_Texture* create_texture(SDL_Renderer* renderer, char* filename, int width, int height);
void load_textures(SDL_Renderer* renderer, SDL_Texture** textures, char** skin_names, int sprite_set);
void free_textures(SDL_Texture** textures);
void free_skin_names(char** skin_names);
