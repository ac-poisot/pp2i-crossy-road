#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../core.h"
#include "sprite_management.h"

void display_text(char* text, int x, int y, int size, SDL_Renderer* renderer, SDL_Color color);
void display_coins(float y, float_list* current_coin, SDL_Renderer* renderer, SDL_Texture** textures);
void display_lane(lane* lane, float lane_count, SDL_Renderer* renderer, SDL_Texture** textures);
void displayPlayer(player player, float cameraY, SDL_Renderer* renderer, SDL_Texture** textures);
void display(displayedData data, SDL_Renderer* renderer, SDL_Texture** textures);
