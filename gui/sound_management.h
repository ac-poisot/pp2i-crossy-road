#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

enum {
    SOUND_COIN,
    SOUND_DEATH,
    SOUND_GAMBLING,
    SOUND_NEW_SKIN,
    SOUND_COUNT,
    MUSIC_MENU,
    MUSIC_MAIN,
    MUSIC_COUNT,
};

void load_sounds(Mix_Chunk** sounds, Mix_Music** music);
void free_sounds(Mix_Chunk** sounds, Mix_Music** music);
