#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "core.h"
#include "gui/display.h"
#include "gui/sprite_management.h"
#include "gui/sound_management.h"
#include "gui/button.h"
#include "minmax.h"

#define AI_AMOUNT 5

#define GAME_SPEED 0.01 // In pixels per frame, speed of the scrolling
#define ANIM_LENGTH 4 // In frames, time it takes for the player to get to the next tile
#define AI_SPEED 6 // In frames, time it takes for the AI to move one tile
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

#define SKINS_PER_LINE (int) ((WIDTH-SKIN_SIDE*2)/(SKIN_SIDE*1.5)+2) // how many skins to be displayed by line
#define PRICE 5

#define FADE_LENGTH 50 // in frames, duration of the transitions
#define GAMBLING_DURATION 120 // in frames, duration of the gambling animation

#define REFRESH_RATE 60

void reset_savefile(void);
void update_savefile(int high_score, int purse, bool* unlocked_skins);
int process_player(player* current_p, displayedData* game, Mix_Chunk** sounds, int* purse, bool* buffer_key_flag, bool is_ai);
int main(void);
