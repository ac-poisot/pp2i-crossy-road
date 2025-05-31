#include "sound_management.h"

void load_sounds(Mix_Chunk** sounds, Mix_Music** music) {
    for (int i = 0; i < SOUND_COUNT; i++) {
        sounds[i] = NULL;
    }
    for (int i = 0; i < MUSIC_COUNT; i++) {
        music[i] = NULL;
    }

    sounds[SOUND_COIN] = Mix_LoadWAV("sound/sfx/coin.wav");

    sounds[SOUND_DEATH] = Mix_LoadWAV("sound/sfx/death.wav");
    sounds[SOUND_GAMBLING] = Mix_LoadWAV("sound/sfx/gambling.wav");
    sounds[SOUND_NEW_SKIN] = Mix_LoadWAV("sound/sfx/new_skin.wav");


    music[MUSIC_MENU] = Mix_LoadMUS("sound/music/menu.wav");
    music[MUSIC_MAIN] = Mix_LoadMUS("sound/music/main.wav");

    if (!sounds[SOUND_COIN] || !sounds[SOUND_DEATH] || !sounds[SOUND_GAMBLING] || !sounds[SOUND_NEW_SKIN]) {
        fprintf(stderr, "Failed to load sound effects: %s\n", Mix_GetError());
    }
    if (!music[MUSIC_MENU] || !music[MUSIC_MAIN]) {
        fprintf(stderr, "Failed to load music: %s\n", Mix_GetError());
    }
}

void free_sounds(Mix_Chunk** sounds, Mix_Music** music) {
    for (int i = 0; i < SOUND_COUNT; i++) {
        if (sounds[i]) {
            Mix_FreeChunk(sounds[i]);
            sounds[i] = NULL;
        }
    }
    for (int i = 0; i < MUSIC_COUNT; i++) {
        if (music[i]) {
            Mix_FreeMusic(music[i]);
            music[i] = NULL;
        }
    }
    Mix_CloseAudio();
}
