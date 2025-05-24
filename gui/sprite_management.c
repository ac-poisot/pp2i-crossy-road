#include "sprite_management.h"

SDL_Texture* create_texture(SDL_Renderer* renderer, char* filename, int width, int height) {
    SDL_Surface *surface = IMG_Load(filename);
    if (surface == NULL) {
        fprintf(stderr, "Failed to load image %s: %s\n", filename, IMG_GetError());
        surface = IMG_Load("sprites/default.png");
    }
    SDL_Surface *resizedSurface = SDL_CreateRGBSurface(0, width, height, surface->format->BitsPerPixel,
        surface->format->Rmask, surface->format->Gmask,
        surface->format->Bmask, surface->format->Amask);

    SDL_BlitScaled(surface, NULL, resizedSurface, NULL);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, resizedSurface);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(resizedSurface);

    return texture;
}

void load_textures(SDL_Renderer* renderer, SDL_Texture** textures, int sprite_set) {

    for (int i=0; i<END_TEXTURES; i++) {
        textures[i] = NULL;
    }

    char prefix[32];
    snprintf(prefix, sizeof(prefix), "sprites/%d/", sprite_set);

    char path[64];

    snprintf(path, sizeof(path), "%sgrass.png", prefix);
    textures[GRASS] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%swater.png", prefix);
    textures[WATER] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%sroad.png", prefix);
    textures[ROAD] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%strack.png", prefix);
    textures[TRACK] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%stree.png", prefix);
    textures[TREE] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%scar1.png", prefix);
    textures[CAR1] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%scar2.png", prefix);
    textures[CAR2] = create_texture(renderer, path, TILE_SIDE*2, TILE_SIDE);

    snprintf(path, sizeof(path), "%slily.png", prefix);
    textures[LILY] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%slog_single.png", prefix);
    textures[LOG_SINGLE] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%slog_edge.png", prefix);
    textures[LOG_EDGE] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%slog_mid.png", prefix);
    textures[LOG_MID] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%strain_edge.png", prefix);
    textures[TRAIN_EDGE] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%strain_mid.png", prefix);
    textures[TRAIN_MID] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%swarning.png", prefix);
    textures[WARNING] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%scloud.png", prefix);
    textures[CLOUD] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    snprintf(path, sizeof(path), "%scoin.png", prefix);
    textures[COIN] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);

    for (int i=1; i<POWERS_END; i++) {
        snprintf(path, sizeof(path), "%spowers/power%d.png", prefix, i);
        textures[POWER_START+i] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);
    }

    snprintf(path, sizeof(path), "%stext/play_button.png", prefix);
    textures[PLAY_BUTTON] = create_texture(renderer, path, BUTTON_WIDTH, BUTTON_HEIGHT);

    snprintf(path, sizeof(path), "%stext/menu_button.png", prefix);
    textures[MENU_BUTTON] = create_texture(renderer, path, BUTTON_WIDTH, BUTTON_HEIGHT);

    snprintf(path, sizeof(path), "%stext/skins_button.png", prefix);
    textures[SKINS_BUTTON] = create_texture(renderer, path, BUTTON_WIDTH, BUTTON_HEIGHT);

    snprintf(path, sizeof(path), "%stext/gamble_button.png", prefix);
    textures[GAMBLE_BUTTON] = create_texture(renderer, path, BUTTON_WIDTH, BUTTON_HEIGHT);

    snprintf(path, sizeof(path), "%slock.png", prefix);
    textures[LOCK] = create_texture(renderer, path, SKIN_SIDE, SKIN_SIDE);

    snprintf(path, sizeof(path), "%stext/title.png", prefix);
    textures[TITLE_CARD] = create_texture(renderer, path, CARD_WIDTH, CARD_HEIGHT);

    snprintf(path, sizeof(path), "%stext/game_over.png", prefix);
    textures[GAME_OVER_CARD] = create_texture(renderer, path, CARD_WIDTH, CARD_HEIGHT);

    snprintf(path, sizeof(path), "%stext/skin_bg.png", prefix);
    textures[SKIN_BG] = create_texture(renderer, path, SKIN_SIDE, SKIN_SIDE);

    for (int i=0; i<SKINS; i++) {
        snprintf(path, sizeof(path), "%sskins/skin%d.png", prefix, i);
        textures[SKIN_START+i] = create_texture(renderer, path, TILE_SIDE, TILE_SIDE);
    }
}

void free_textures(SDL_Texture** textures) {
    for (int i=0; i<END_TEXTURES; i++) {
        if (textures[i] != NULL) {
            SDL_DestroyTexture(textures[i]);
        }
    }
}
