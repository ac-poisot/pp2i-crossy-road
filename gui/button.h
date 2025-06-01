#pragma once

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct button {
    int x;
    int y;
    int width;
    int height;
    int texture;
} button;

void display_button(button b, SDL_Renderer* renderer, SDL_Texture** textures);
bool button_clicked(button b, SDL_Event event);
