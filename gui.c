#include "gui.h"

void reset_savefile(void) {
    FILE *save_data = fopen("save/data.txt", "w");
    fprintf(save_data, "0\n0");
    for (int i=1; i<SKINS; i++) {
        fprintf(save_data, "\n0");
    }
    fclose(save_data);
}

void update_savefile(int high_score, int purse, bool* unlocked_skins) {
    FILE *save_data = fopen("save/data.txt", "w");
    fprintf(save_data, "%d\n%d", high_score, purse);
    for (int i=1; i<SKINS; i++) {
        fprintf(save_data, "\n%d", unlocked_skins[i]);
    }
    fclose(save_data);
}


int main(void) {
    obstacle* didier = malloc(sizeof(obstacle));
    didier->next = NULL;
    didier->prev = NULL;
    didier->size = -12;
    didier->x = 0;
    struct timespec remaining, request = { 0, 1000000000/REFRESH_RATE}; // ~1 frame at REFRESH_RATE fps
    time_t SEED = time(NULL);
    srand(SEED);

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1)
    {
       printf("%s", Mix_GetError());
    }
    Mix_AllocateChannels(10);

    Mix_Chunk *sounds[SOUND_COUNT];
    Mix_Music *music[MUSIC_COUNT];
    load_sounds(sounds, music);

    Mix_Volume(1, MIX_MAX_VOLUME/2);
    Mix_VolumeMusic(MIX_MAX_VOLUME/2);

    bool sound_played = false;

    SDL_Window *window = SDL_CreateWindow("Crossy Road", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;
    int running = 1;
    bool action = false;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    int game_state = MENU;
    int player_skin = 0;
    int purse = 50;
    bool unlocked_skins[SKINS] = {true};
    for (int i=1; i<SKINS; i++) {
        unlocked_skins[i] = false;
    }
    unlocked_skins[0] = true;

    // Game-specific variables
    displayedData game = {0, NULL, NULL, {0,0, 0, 0, 0}, 0, 0};

    bool drown_flag = false; // keeps track of whether the player is fully in empty waters or not
    bool blocked_path = false; // whether the path is currently blocked by a tree or not
    obstacle* on_log = NULL; // the log on which the player is
    float x_offset = 0; // in tiles per frame, speed at which x coordinate must be changed during the animation to place the player on top of a tile (specifically for going on and off logs)
    float liftboost = 0; // in tiles per frame, speed at which the player is being carried (specifically for logs)
    int high_score = 0;

    int player_anim = 0; // timer for player animation (0 = stopped, anything else = moving)
    int buffer = 0; // stores the next movement to be performed
    bool buffer_key_flag = true; // whether the key for the last movement has been released or not
    
    int skin_to_unlock = 0;
    // Menu and transition variables
    displayedData demo = init_game(GAME_HEIGHT);
    demo.player.skin = -1;
    int fade = FADE_LENGTH;
    
    int power_time = 0;//Time remaining for the power

    int ai_choice = 0;
    int sprite_set = 1;

    SDL_Texture** textures = (SDL_Texture**) malloc((END_TEXTURES)*(sizeof(SDL_Texture*)));
    char** skin_names = (char**) malloc((SKINS)*(sizeof(char*)));
    load_textures(renderer, textures, skin_names, sprite_set);

    FILE *save_data = fopen("save/data.txt", "a+");
    if (save_data == NULL) {
        printf("Error opening save file. Creating a new one.\n");
        reset_savefile();
        save_data = fopen("save/data.txt", "r+");
    }
    else {
        fseek(save_data, 0, SEEK_END);
        long filesize = ftell(save_data);
        rewind(save_data);
        if (filesize == 0) {
            reset_savefile();
        } else {
            fscanf(save_data, "%d\n", &high_score);
            fscanf(save_data, "%d\n", &purse);
            for (int i=1; i<SKINS; i++) {
                int unlocked = 0;
                fscanf(save_data, "%d\n", &unlocked);
                if (unlocked && unlocked != 1) {
                    printf("Error detected in savefile. Erasing savefile.");
                    reset_savefile();
                    break;
                }
                unlocked_skins[i] = unlocked;
            }
        }
    }
    fclose(save_data);
    

    // Buttons

    button skin_buttons[SKINS] = {{0, 0, SKIN_SIDE, SKIN_SIDE, SKIN_BG}};

    for (int i=0; i<SKINS; i++) {
        skin_buttons[i].x = ((i%SKINS_PER_LINE)*SKIN_SIDE*1.5)+SKIN_SIDE;
        skin_buttons[i].y = (i/SKINS_PER_LINE*SKIN_SIDE*1.5)+SKIN_SIDE;
        skin_buttons[i].width = SKIN_SIDE;
        skin_buttons[i].height = SKIN_SIDE;
        skin_buttons[i].texture = SKIN_BG;
    }

    while (running) {

        action = SDL_PollEvent(&event);
        SDL_FlushEvent(SDL_MOUSEMOTION);

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
        case MENU: {
            if (!Mix_PlayingMusic()) {
                Mix_PlayMusic(music[MUSIC_MENU], -1);
            }
            // Demo background
            SDL_RenderClear(renderer);
            display(demo, renderer, textures, sprite_set, power_time, TIME_POWER);
    
            demo = move_camera(demo, GAME_SPEED*5);
    
            lane* current_lane = demo.camera_first_lane;

            while (current_lane->next != NULL) {
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

                current_lane = current_lane->next;
            }

            // Display buttons

            button play = {(WIDTH-BUTTON_WIDTH)/2, (HEIGHT-BUTTON_HEIGHT)/2, BUTTON_WIDTH, BUTTON_HEIGHT, PLAY_BUTTON};
            button skins = {(WIDTH-BUTTON_WIDTH)/2, (HEIGHT-BUTTON_HEIGHT)/2 + 1.5*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, SKINS_BUTTON};
            button gamble = {(WIDTH-BUTTON_WIDTH)/2, (HEIGHT-BUTTON_HEIGHT)/2 + 3*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, GAMBLE_BUTTON};

            display_button(play, renderer, textures);
            display_button(skins, renderer, textures);
            if (purse >= PRICE) {
                display_button(gamble, renderer, textures);
            } else {
                SDL_Rect spriteRect = {0, 0, BUTTON_WIDTH, BUTTON_HEIGHT};
                SDL_Rect destRect = {gamble.x, gamble.y, BUTTON_WIDTH, BUTTON_HEIGHT};
                SDL_RenderCopy(renderer, textures[LOCK], &spriteRect, &destRect);
            }

            // AI choice
            button ai = {(WIDTH+BUTTON_WIDTH)/2, (HEIGHT-BUTTON_HEIGHT)/2, BUTTON_HEIGHT, BUTTON_HEIGHT, CLOUD};
            display_button(ai, renderer, textures);

            char aiText[2];
            if (ai_choice) {
                sprintf(aiText, "%d", ai_choice);
            } else {
                sprintf(aiText, "X");
            }

            display_text(aiText, (WIDTH+BUTTON_WIDTH)/2 + BUTTON_HEIGHT/2 - 24, (HEIGHT-BUTTON_HEIGHT)/2 - 20, BUTTON_HEIGHT*0.9, renderer, black, "Symtext.ttf");
            
            // Sprite set choice
            button sprite_set_button = {BUTTON_HEIGHT/2, HEIGHT - BUTTON_HEIGHT*1.5, BUTTON_HEIGHT, BUTTON_HEIGHT, CLOUD};
            display_button(sprite_set_button, renderer, textures);

            SDL_Rect spriteRect = {0, 0, SKIN_SIDE, SKIN_SIDE};
            SDL_Rect destRect = {sprite_set_button.x+(sprite_set_button.width/2)-SKIN_SIDE/2, sprite_set_button.y+(sprite_set_button.height/2)-SKIN_SIDE/2, SKIN_SIDE, SKIN_SIDE};
            SDL_RenderCopy(renderer, textures[SKIN_START], &spriteRect, &destRect);

            button reset = {WIDTH-BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, CLOUD};
            display_button(reset, renderer, textures);
            display_text("RESET", WIDTH-BUTTON_WIDTH+28, BUTTON_HEIGHT+20, 40, renderer, black, "Symtext.ttf");


            switch (event.type) {
                case SDL_MOUSEBUTTONUP:
                    if (button_clicked(play, event)) {
                            game_state = MENU_TO_GAME;
                        }
                    else if (button_clicked(skins, event)) {
                        game_state = SKIN_SELECT;
                    }
                    else if (button_clicked(gamble, event) && purse >= PRICE) {
                        Mix_HaltMusic();
                        Mix_PlayChannel(1, sounds[SOUND_GAMBLING], 0);
                        purse -= PRICE;
                        fade = GAMBLING_DURATION;
                        game_state = GAMBLING;
                        skin_to_unlock = (rand() % (SKINS-1))+1;
                    } else if (button_clicked(reset, event)) {
                        reset_savefile();
                        high_score = 0;
                        purse = 0;
                        for (int i=0; i<SKINS; i++) {
                            unlocked_skins[i] = 0;
                        }
                    }
                    else if (button_clicked(ai, event)) {
                        ai_choice = (ai_choice + 1) % AI_AMOUNT;
                    }
                    else if (button_clicked(sprite_set_button, event)) {
                        sprite_set = (sprite_set + 1) % 2;
                        free_textures(textures);
                        free_skin_names(skin_names);
                        load_textures(renderer, textures, skin_names, sprite_set);
                    }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        game_state = MENU_TO_GAME;
                    }
            }

            SDL_Rect spriteRect2 = {0, 0, CARD_WIDTH, CARD_HEIGHT};
            SDL_Rect destRect2 = {(WIDTH-CARD_WIDTH)/2, (HEIGHT/2-CARD_HEIGHT)/2, CARD_WIDTH, CARD_HEIGHT};
            SDL_RenderCopy(renderer, textures[TITLE_CARD], &spriteRect2, &destRect2);

            // Display high score
            char highScoreText[20];
            sprintf(highScoreText, "High Score: %d", high_score);
            display_text(highScoreText, 15, 15, 24, renderer, white, "Symtext.ttf");
            // Display purse
            char purseText[20];
            sprintf(purseText, "%d$", purse);
            display_text(purseText, WIDTH-24*((int)(log10(purse))+2)-15, 15, 24, renderer, white, "Symtext.ttf");

            break;
        }

        case MENU_TO_GAME: {
            
            if (fade) {
                Mix_FadeOutMusic(FADE_LENGTH*1000/REFRESH_RATE);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

                if (fade >= FADE_LENGTH/2) {
                    SDL_Rect bg = {0, 0, WIDTH, (int) ((float) (FADE_LENGTH-fade)/FADE_LENGTH*HEIGHT*2)};
                    SDL_RenderFillRect(renderer, &bg);

                    if (fade == FADE_LENGTH/2) {
                        free_lanes(game.first_lane);
                        game = init_game(GAME_HEIGHT);
                        on_log = NULL;
                        x_offset = 0;
                        liftboost = 0;
                        player_anim = 0;
                        game.player.skin = player_skin;
                        buffer = 0;
                        buffer_key_flag = true;
                        power_time = 0;
                    }
                } else {
                    display(game, renderer, textures, sprite_set, power_time, TIME_POWER);
                    SDL_Rect bg = {0, 0, WIDTH, (int) ((float) (fade)/FADE_LENGTH*HEIGHT*2)};
                    SDL_RenderFillRect(renderer, &bg);
                }
                fade--;
            } else {
                game_state = GAME;
                fade = FADE_LENGTH;
            }
            break;
        }

        case GAME:
            if (!Mix_PlayingMusic()) {
                Mix_PlayMusic(music[MUSIC_MAIN], -1);
            }
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
    
    
            display(game, renderer, textures, sprite_set, power_time, TIME_POWER);
            // Display the purse
            char purseText[20];
            sprintf(purseText, "%d$", purse);
            display_text(purseText, WIDTH-24*((int)(log10(purse))+2)-15, 15, 24, renderer, white, "Symtext.ttf");
    
            game = move_camera(game, GAME_SPEED);
    
            lane* current_lane = game.camera_first_lane;
            lane* player_top_lane = NULL; // lane on or above the player
            lane* player_bottom_lane = NULL; // lane below the player

            while (current_lane->next != NULL) {
    
                // Update lanes
    
                switch (current_lane->type) {//Power 2
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
                if (current_lane->y == ceil(game.player.y)) {
                    player_top_lane = current_lane;
                } else if (current_lane->y == floor(game.player.y)) {
                    player_bottom_lane = current_lane;
                }
                current_lane = current_lane->next;
            }

            // Collision check
            blocked_path = false;

            if (player_top_lane != NULL) {

                // Check for collisions with coins
                float_list* collided_coin =  collides_coin(player_top_lane, game);
                if (collided_coin != NULL) {
                    if(collided_coin->power == 0){
                        Mix_PlayChannel(-1, sounds[SOUND_COIN], 0);
                        if (game.player.power == CRESUS) {
                            purse += CRESUS_MODIF;
                        }
                        else {
                            purse++;
                        }
                    }
                    else {
                        if(collided_coin->power == ECOLO){
                            power_time =1;
                            game = power4(game);
                            on_log = NULL;
                            liftboost = 0;
                            break;
                        }
                        else {  
                            game.player.power = collided_coin->power;
                            power_time = TIME_POWER*REFRESH_RATE;
                        }
                    }
                    
                }


                if (player_top_lane->type == WATER && game.player.power != XIV) {
                    drown_flag = true; // only matters for the top lane, which is the lane the player is on or is going to
                    }
                else {
                    drown_flag = false;
                    }

                if (x_offset == 0 && on_log == NULL && player_top_lane->type == WATER && player_top_lane->speed != 0 && game.player.power == XIV) {
                        liftboost = player_top_lane->speed;
                        on_log = didier;
                        x_offset = 0.0001;
                    }

                //Check for collisions with obstacles
                obstacle* collided_obstacle = collides(player_top_lane, game);

                if(collided_obstacle != NULL) {
                    switch (player_top_lane->type) {
                        case GRASS:
                        if (game.player.power != TANK){
                            if (x_offset == 0) {
                                blocked_path = true;
                            }
                        }
                        else {
                            // Faire une fonction pour couper les arbres / Train / Voiture
                        }
                        break;
                        case TRACK:
                        if (game.player.power != TANK){
                            game_state = GAME_OVER;
                        }
                        else {
                            game = tank_train(game);
                            collided_obstacle = NULL;
                        }
                        break;
                        case ROAD:
                        if (game.player.power != TANK){
                            game_state = GAME_OVER;
                        }
                        else {
                            game = tank_road(game,  collided_obstacle,player_top_lane);
                            collided_obstacle = NULL;
                        }
                        break;
                        case WATER:
                        drown_flag = false; // an obstacle has been found, player now should not drown
                        if (player_top_lane->speed != 0 && game.player.power != XIV) { // if logs are on the lane

                            // case where we are going up to different log than before or from ground
                            if (on_log != collided_obstacle && on_log != collided_obstacle->next && game.player.orientation == UP) {
                                int pos_on_log = (int) round(game.player.x - collided_obstacle->x); // in tiles, position of the player relative to the log
                                on_log = collided_obstacle;
                                liftboost = player_top_lane->speed;
                                x_offset = ((collided_obstacle->x + pos_on_log)-game.player.x)/(ANIM_LENGTH);

                                // in case the player is too far right to be on the log that's being collided with, check if there is a log right next to it
                                if (pos_on_log == collided_obstacle->size && collided_obstacle->next != NULL && floor(collided_obstacle->x + collided_obstacle->size) == floor(collided_obstacle->next->x)) {
                                    on_log = collided_obstacle->next;
                                    
                                    x_offset = (collided_obstacle->next->x - game.player.x)/(ANIM_LENGTH);
                                    
                                }
                            }
                        }
                        break;
                        default:
                        break;
                    }
                }

                // leaving a log
                if (on_log != NULL && !(player_top_lane->type == WATER && player_top_lane->speed != 0) && game.player.orientation == UP) {
                    
                    if (collided_obstacle != NULL && player_top_lane->type == GRASS) {
                        blocked_path = true;
                    } else {
                        liftboost = 0;
                        x_offset = (round(game.player.x) - game.player.x)/ANIM_LENGTH;
                        on_log = NULL;
                    }
                }
            }

            if (player_bottom_lane != NULL) {
                // Check for collisions with coins
                float_list* collided_coin =  collides_coin(player_bottom_lane, game);
                if (collided_coin != NULL) {
                    if(collided_coin->power == 0){
                        Mix_PlayChannel(-1, sounds[SOUND_COIN], 0);
                        if (game.player.power == CRESUS) {
                            purse += CRESUS_MODIF;
                        }
                        else {
                            purse++;
                        }
                    }
                    else {
                        if(collided_coin->power == ECOLO){
                            power_time =1;
                            game = power4(game);
                            on_log = NULL;
                            liftboost = 0;
                            break;
                        }
                        else {
                            game.player.power = collided_coin->power;
                            power_time = TIME_POWER*REFRESH_RATE;
                        }
                    }
                    
                }

                if (x_offset == 0 && on_log == NULL && player_bottom_lane->type == WATER && player_bottom_lane->speed != 0 && game.player.power == XIV) {
                    liftboost = player_bottom_lane->speed;
                    on_log = didier;
                    x_offset = 0.0001;
                }
                // Check for collisions with obstacles
                obstacle* collided_obstacle = collides(player_bottom_lane, game);

                if(collided_obstacle != NULL) {
                    switch (player_bottom_lane->type) {
                        case GRASS:
                        if (game.player.power != TANK){
                            if (x_offset == 0) {
                                blocked_path = true;
                            }
                        }
                        else {
                            // Faire une fonction pour couper les arbres / Train / Voiture
                        }
                        break;
                        case TRACK:
                        if (game.player.power != TANK){
                            game_state = GAME_OVER;
                        }
                        else {
                            game = tank_train(game);
                            collided_obstacle = NULL;
                        }
                        break;
                        case ROAD:
                        if (game.player.power != TANK){
                            game_state = GAME_OVER;
                        }
                        else {
                            game = tank_road(game,collided_obstacle,player_bottom_lane);
                            collided_obstacle = NULL;
                        }
                        break;
                        case WATER:
                        if (player_bottom_lane->speed != 0 && game.player.power != XIV) {

                            // case where we are going down to different log than before or from ground
                            if (on_log != collided_obstacle && on_log != collided_obstacle->next && game.player.orientation == DOWN) {
                                int pos_on_log = (int) round(game.player.x - collided_obstacle->x);
                                on_log = collided_obstacle;
                                liftboost = player_bottom_lane->speed;
                                x_offset = ((collided_obstacle->x + pos_on_log)-game.player.x)/(ANIM_LENGTH);

                                // in case the player is too far right to be on the log that's being collided with, check if there is a log right next to it
                                if (pos_on_log == collided_obstacle->size && collided_obstacle->next != NULL && floor(collided_obstacle->x + collided_obstacle->size) == floor(collided_obstacle->next->x)) {
                                    on_log = collided_obstacle->next;
                                    x_offset = (collided_obstacle->next->x - game.player.x)/(ANIM_LENGTH);
                               }
                            }
                        }
                        break;
                        default:
                        break;
                    }
                }

                if (on_log != NULL && !(player_bottom_lane->type == WATER && player_bottom_lane->speed != 0) && game.player.orientation == DOWN) {                
                    
                    if (collided_obstacle != NULL && player_bottom_lane->type == GRASS) {
                        blocked_path = true;
                    } else {
                        liftboost = 0;
                        x_offset = (round(game.player.x) - game.player.x)/ANIM_LENGTH;
                        on_log = NULL;
                    }
                }
            

            }

            // screen edges
            if (game.player.x < UNPLAYABLE_WIDTH || game.player.x > LANE_WIDTH - UNPLAYABLE_WIDTH - 1) {
                blocked_path = true;
            } 

            // Update player

            game.player.x += liftboost; // apply liftboost

            // a new action needs to be performed!
            if (!player_anim && buffer && buffer_key_flag && !blocked_path && !drown_flag) {
                game.player.orientation = buffer;
                player_anim = ANIM_LENGTH;// FOR TANK, should be  double
                buffer_key_flag = false;
                buffer = 0;
            }

            // player is currently moving, update everything accordingly
            if (player_anim) {
                if (blocked_path) {
                    player_anim = 0;
                } else {
                    switch (game.player.orientation) {
                        case UP:
                            game.player.y += PLAYER_SPEED;
                            if (game.cameraY - game.player.y < (GAME_HEIGHT/4)) {
                                game = move_camera(game, PLAYER_SPEED); // move the camera if the player is too high
                            }
                            break;
                        case DOWN:
                            game.player.y -= PLAYER_SPEED;
                            break;
                        case RIGHT:
                            game.player.x += PLAYER_SPEED;
                            break;
                        case LEFT:
                            game.player.x -= PLAYER_SPEED;
                            break;
                    }
                    game.player.x += x_offset;
                    player_anim--;
                }

                // if action is ending, recenter the player and reset variables
                if (!player_anim) {
                    game.player.y = round(game.player.y); // Recenter player position to avoid float drifting
                    buffer_key_flag = true;
                    x_offset = 0;

                    if (on_log == NULL) {
                        game.player.x = round(game.player.x);  // Recenter player position to avoid float drifting
                    }
                }

            }

            // checking for game over

            if (game.player.y < game.cameraY - GAME_HEIGHT || // player is too low
                (drown_flag && !player_anim) || // player is drowning
                (game.player.x < UNPLAYABLE_WIDTH && on_log != NULL) || // player is being carried offscreen
                (game.player.x > LANE_WIDTH - UNPLAYABLE_WIDTH - 1 && on_log != NULL)) // player is being carreid offscreen
            {
                game_state = GAME_OVER;
            }

            if (!player_anim && on_log != NULL && game.player.power != XIV) {
                if (game.player.x > on_log->x + on_log->size - 0.1) { // check if player is too far right on the log it's currently on
                    if (on_log->next != NULL && floor(on_log->x + on_log->size) == floor(on_log->next->x)) { // check if player can move right to a different adjacent log
                        on_log = on_log->next;
                    } else {
                        game_state = GAME_OVER; // if not, game over
                    }
                }
                else if (game.player.x+0.1 < on_log->x) { // check if player is too far left on the log it's currently on
                    if (on_log->prev != NULL && floor(on_log->prev->x + on_log->prev->size) == floor(on_log->x)) { // check if player can move left to a different adjacent log
                        on_log = on_log->prev;
                    } else {
                        game_state = GAME_OVER; // if not, game over
                    }
                }
            }
            break;
        case GAME_OVER: {
            Mix_HaltMusic();
            if (!sound_played) {
                sound_played = true;
                Mix_PlayChannel(-1, sounds[SOUND_DEATH], 0);
            }

            button menu_button = {(WIDTH-BUTTON_WIDTH)/2, (HEIGHT-BUTTON_HEIGHT)/2, BUTTON_WIDTH, BUTTON_HEIGHT, MENU_BUTTON};
            display_button(menu_button, renderer, textures);

            switch (event.type) {
                case SDL_MOUSEBUTTONUP:
                    if (button_clicked(menu_button, event)) {
                            game_state = GO_TO_MENU;
                        }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        game_state = GO_TO_MENU;
                    }
            }
            SDL_Rect spriteRect2 = {0, 0, CARD_WIDTH, CARD_HEIGHT};
            SDL_Rect destRect2 = {(WIDTH-CARD_WIDTH)/2, (HEIGHT/2-CARD_HEIGHT)/2, CARD_WIDTH, CARD_HEIGHT};
            SDL_RenderCopy(renderer, textures[GAME_OVER_CARD], &spriteRect2, &destRect2);
            break;
        }
        case GO_TO_MENU: {
            if (fade) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

                if (fade >= FADE_LENGTH/2) {
                    SDL_Rect bg = {0, 0, WIDTH, (int) ((float) (FADE_LENGTH-fade)/FADE_LENGTH*HEIGHT*2)};
                    SDL_RenderFillRect(renderer, &bg);
                } else {
                    display(demo, renderer, textures, sprite_set, power_time, TIME_POWER);
                    SDL_Rect bg = {0, 0, WIDTH, (int) ((float) (fade)/FADE_LENGTH*HEIGHT*2)};
                    SDL_RenderFillRect(renderer, &bg);
                }
                fade--;
            } else {
                if (high_score < (int) game.player.y) {
                    high_score = (int) game.player.y;
                }
                update_savefile(high_score, purse, unlocked_skins);
                game_state = MENU;
                fade = FADE_LENGTH;
                sound_played = false;
            }
            break;
        }
        case SKIN_SELECT: {
            SDL_RenderClear(renderer);
            display(demo, renderer, textures, sprite_set, power_time, TIME_POWER);
            demo = move_camera(demo, GAME_SPEED*5);
            lane* current_lane = demo.camera_first_lane;
            while (current_lane->next != NULL) {
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
                current_lane = current_lane->next;
            }

            button menu_button = {0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, MENU_BUTTON};
            display_button(menu_button, renderer, textures);

            for (int i=0; i<SKINS; i++) {

                char* skin_name = skin_names[i];

                if (player_skin == i) {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
                    SDL_Rect bg = {skin_buttons[i].x, skin_buttons[i].y, SKIN_SIDE, SKIN_SIDE};
                    SDL_RenderFillRect(renderer, &bg);
                }

                display_button(skin_buttons[i], renderer, textures);
                SDL_Rect spriteRect = {0, 0, SKIN_SIDE, SKIN_SIDE};
                SDL_Rect destRect = {skin_buttons[i].x, skin_buttons[i].y, SKIN_SIDE, SKIN_SIDE};
                SDL_RenderCopy(renderer, textures[SKIN_START+i], &spriteRect, &destRect);

                if (!unlocked_skins[i]) {
                    SDL_Rect lockRect = {skin_buttons[i].x, skin_buttons[i].y, SKIN_SIDE, SKIN_SIDE};
                    SDL_RenderCopy(renderer, textures[LOCK], &spriteRect, &lockRect);
                    skin_name = "???";
                }

                display_text(skin_name, skin_buttons[i].x, skin_buttons[i].y + SKIN_SIDE, 24, renderer, white, "Symtext.ttf");
            }
            switch (event.type) {
                case SDL_MOUSEBUTTONUP:
                    if (button_clicked(menu_button, event)) {
                            game_state = MENU;
                        } else {     
                        for (int i=0; i<SKINS; i++) {
                            if (button_clicked(skin_buttons[i], event)) {
                                if (unlocked_skins[i]) {
                                    player_skin = i;
                                }
                            }
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        game_state = MENU;
                    }
            }
            break;
        }
        case GAMBLING: {
            SDL_RenderClear(renderer);
            display(demo, renderer, textures, sprite_set, power_time, TIME_POWER);
            demo = move_camera(demo, GAME_SPEED*5);
            lane* current_lane = demo.camera_first_lane;
            while (current_lane->next != NULL) {
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
                current_lane = current_lane->next;
            }
            
            // Display unlocked skin

            int icon_size = SKIN_SIDE*4*((float) GAMBLING_DURATION-fade)/GAMBLING_DURATION;

            SDL_Rect spriteRect = {0, 0, SKIN_SIDE, SKIN_SIDE};
            SDL_Rect destRect = {WIDTH/2-icon_size/2, HEIGHT/2-icon_size/2, icon_size, icon_size};
            SDL_RenderCopy(renderer, textures[SKIN_START+skin_to_unlock], &spriteRect, &destRect);

            fade--;
            if (!fade) {
                fade = GAMBLING_DURATION;
                game_state = GAMBLED;
                if (!unlocked_skins[skin_to_unlock]) {
                    Mix_PlayChannel(-1, sounds[SOUND_NEW_SKIN], 0);
                    char* skin_name = skin_names[skin_to_unlock];
                    display_text("New skin unlocked!", WIDTH/2-280, HEIGHT/2+icon_size/2, 50, renderer, white, "Symtext.ttf");
                    display_text(skin_name, WIDTH/2-100, HEIGHT/2+icon_size/2+50,50, renderer, white, "Symtext.ttf");
                } else {
                    display_text("You already have this skin!", WIDTH/2-400, HEIGHT/2+icon_size/2, 45, renderer, white, "Symtext.ttf");
                }
                unlocked_skins[skin_to_unlock] = true;
                update_savefile(high_score, purse, unlocked_skins);
            }
            break;
        }
        case GAMBLED: {

            button menu_button = {0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, MENU_BUTTON};
            display_button(menu_button, renderer, textures);

            switch (event.type) {
                case SDL_MOUSEBUTTONUP:
                    if (button_clicked(menu_button, event)) {
                            game_state = MENU;
                        }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        game_state = MENU;
                    }
            }
            break;
        }
        default:
            break;
        }

        SDL_RenderPresent(renderer);
        if (power_time>0){
            power_time--;
        }//Decrease time remaining for the power;
         if (power_time<=0) {
            game.player.power = 0;
        }
        nanosleep(&request, &remaining);
    }

    free_textures(textures);
    free(textures);
    free_skin_names(skin_names);
    free(skin_names);

    free_lanes(game.first_lane);
    free_lanes(demo.first_lane);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    free(didier);

    free_sounds(sounds, music);

    IMG_Quit();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
