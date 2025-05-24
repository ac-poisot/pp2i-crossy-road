#include "sprite_management.h"

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

    for (int i=0; i<END_TEXTURES; i++) {
        textures[i] = NULL;
    }

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
    textures[CLOUD] = create_texture(renderer, "sprites/cloud.png", TILE_SIDE, TILE_SIDE);
    textures[COIN] = create_texture(renderer, "sprites/coin.png", TILE_SIDE, TILE_SIDE);
    for (int i=1; i<POWERS_END; i++) {
        char s[30];
        sprintf(s, "sprites/powers/power%d.png", i);
        textures[POWER_START+i] = create_texture(renderer, s, TILE_SIDE, TILE_SIDE);
    }

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

void free_textures(SDL_Texture** textures) {
    for (int i=0; i<END_TEXTURES; i++) {
        if (textures[i] != NULL) {
            SDL_DestroyTexture(textures[i]);
        }
    }

    free(textures);
}
