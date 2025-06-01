#include "display.h"

void display_text(char* text, int x, int y, int size, SDL_Renderer* renderer, SDL_Color color, char* font, bool bg) {
    
    char font_path[256];
    snprintf(font_path, sizeof(font_path), "fonts/%s", font);
    TTF_Font* ttf_font = TTF_OpenFont(font_path, size);
    if (ttf_font == NULL) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        return;
    }
    SDL_Surface* textSurface = TTF_RenderText_Solid(ttf_font, text, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};

    if (bg) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_Rect bgRect = {x-5, y-5, textSurface->w+10, textSurface->h+10};
        SDL_RenderFillRect(renderer, &bgRect);
    }

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(ttf_font);
}

void display_coins(float y, float_list* current_coin, SDL_Renderer* renderer, SDL_Texture** textures) {
    while (current_coin != NULL) {
        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
        SDL_Rect destRect = {current_coin->val*TILE_SIDE, y*TILE_SIDE, TILE_SIDE, TILE_SIDE};

        int texture;
        if (current_coin->power == 0) {
            texture = COIN;
        } else {
            texture = POWER_START + current_coin->power;
        }
        SDL_RenderCopy(renderer, textures[texture], &spriteRect, &destRect);
        current_coin = current_coin->next;
    }
}

void display_lane(lane* lane, float lane_count, SDL_Renderer* renderer, SDL_Texture** textures) {

    float screen_y = GAME_HEIGHT - lane_count;

    for (int i = 0; i < LANE_WIDTH; i++) {
        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
        SDL_Rect destRect = {i*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
        if (lane->type == WATER && (i*4+lane->y)%10 == 0) {
            SDL_RenderCopy(renderer, textures[DUCK], &spriteRect, &destRect);
        } else {
            SDL_RenderCopy(renderer, textures[lane->type], &spriteRect, &destRect);
        }
    }

    // Display coins
    if (lane->type != WATER) {
        display_coins(screen_y, lane->coins, renderer, textures);
    }

    // Display obstacles
    obstacle* current_obstacle = lane->obstacles;
    int flip = (lane->speed < 0);

    if (current_obstacle != NULL) {
        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
    
        while (current_obstacle != NULL) {
            SDL_Rect destRect = {current_obstacle->x*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};

            switch (lane->type)
            {
            case GRASS:
                SDL_RenderCopy(renderer, textures[TREE], &spriteRect, &destRect);
                break;
            case WATER:
                if (lane->speed == 0) {
                    SDL_RenderCopy(renderer, textures[LILY], &spriteRect, &destRect);
                    } else {
                        if (current_obstacle->size > 1) {
                            SDL_RenderCopyEx(renderer, textures[LOG_EDGE], &spriteRect, &destRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                            for (int i=1; i<(current_obstacle->size-1); i++) {
                                SDL_Rect destRect2 = {(current_obstacle->x+i)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                                SDL_RenderCopy(renderer, textures[LOG_MID], &spriteRect, &destRect2);
                            }
                            SDL_Rect destRect3 = {(current_obstacle->x+current_obstacle->size-1)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                            SDL_RenderCopy(renderer, textures[LOG_EDGE], &spriteRect, &destRect3);
                        } else {
                            SDL_RenderCopy(renderer, textures[LOG_SINGLE], &spriteRect, &destRect);
                        }
                }   
                break;
            case TRACK:
                SDL_RenderCopyEx(renderer, textures[TRAIN_EDGE], &spriteRect, &destRect, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                for (int i=1; i<(current_obstacle->size-1); i++) {
                    SDL_Rect destRect2 = {(current_obstacle->x+i)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                    SDL_RenderCopy(renderer, textures[TRAIN_MID], &spriteRect, &destRect2);
                }
                SDL_Rect destRect3 = {(current_obstacle->x+current_obstacle->size-1)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                SDL_RenderCopyEx(renderer, textures[TRAIN_EDGE], &spriteRect, &destRect3, 0.0, NULL, 0);

                if (!flip) {
                    if (current_obstacle->x + current_obstacle->size > -WARNING_TIME*TRAIN_SPEED && current_obstacle->x + current_obstacle->size < 0) {
                        SDL_Rect warningRect = {0, 0, TILE_SIDE, TILE_SIDE};
                        SDL_Rect destRect2 = {0, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                        SDL_RenderCopy(renderer, textures[WARNING], &warningRect, &destRect2);
                    }
                } else {
                    if (current_obstacle->x < LANE_WIDTH + WARNING_TIME*TRAIN_SPEED && current_obstacle->x > LANE_WIDTH) {
                        SDL_Rect warningRect = {0, 0, TILE_SIDE, TILE_SIDE};
                        SDL_Rect destRect2 = {(LANE_WIDTH-1)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                        SDL_RenderCopy(renderer, textures[WARNING], &warningRect, &destRect2);
                    }
                }
                break;
            case ROAD:
                if (current_obstacle->size == 1) {
                    SDL_RenderCopyEx(renderer, textures[CAR1], &spriteRect, &destRect, 0.0, NULL, flip);
                } else {
                    SDL_Rect spriteRect2 = {0, 0, TILE_SIDE*2, TILE_SIDE};
                    SDL_Rect destRect2 = {current_obstacle->x*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE*2, TILE_SIDE};
                    SDL_RenderCopyEx(renderer, textures[CAR2], &spriteRect2, &destRect2, 0.0, NULL, flip);
                }
                break;  
            default:
                break;
            }
        current_obstacle = current_obstacle->next;
        }

    }

    if (lane->type == WATER) {
        display_coins(screen_y, lane->coins, renderer, textures);
    }
}

void displayPlayer(player player, float cameraY, SDL_Renderer* renderer, SDL_Texture** textures, int skin_set) {
    
    if (skin_set == 0) {
        SDL_Rect destRect = {player.x*TILE_SIDE, (cameraY- player.y)*TILE_SIDE, TILE_SIDE, TILE_SIDE};
        SDL_Rect spriteRect = {0, 0, SKIN_SIDE, SKIN_SIDE};
        SDL_RenderCopyEx(renderer, textures[player.skin+SKIN_START], &spriteRect, &destRect, player.orientation, NULL, false);
    } else {
        SDL_Rect destRect = {player.x*TILE_SIDE-TILE_SIDE*0.2, (cameraY- player.y)*TILE_SIDE-TILE_SIDE*0.5, TILE_SIDE*1.5, TILE_SIDE*1.5};
        SDL_Rect spriteRect = {SKIN_SIDE*(((player.orientation+180)/90)%4), 0, SKIN_SIDE, SKIN_SIDE};
        SDL_RenderCopy(renderer, textures[player.skin+SKIN_START], &spriteRect, &destRect);
    }
}

void display(displayedData data, SDL_Renderer* renderer, SDL_Texture** textures, int skin_set, int power_duration) {
    // Display the lanes
    lane* current_lane = data.camera_first_lane;
    
    float lane_count = (int)(data.cameraY) - data.cameraY;
    while (lane_count <= GAME_HEIGHT+1) {
        display_lane(current_lane, lane_count, renderer, textures);
        current_lane = current_lane->next;
        lane_count++;
    }

    if (data.player.skin != -1) {
        displayPlayer(data.player, data.cameraY, renderer, textures, skin_set);
    }

    if (data.ai.skin != -1) {
        displayPlayer(data.ai, data.cameraY, renderer, textures, skin_set);
    }

    // Display score
    char scoreText[20];

    if (data.player.skin == -1) {
        sprintf(scoreText, "Score: %d", (int) data.ai.y);
    } else {
        sprintf(scoreText, "Score: %d", (int) data.player.y);
    }
    SDL_Color white = {255, 255, 255, 255};
    
    if (data.player.skin != -1 || data.ai.skin != -1) {
        display_text(scoreText, 15, 15, 24, renderer, white, "Symtext.ttf", true);
    }



    if (data.player.skin != -1 && data.player.power != 0 && !(data.player.power_time < power_duration*60/5 && (data.player.power_time%10) < 5)) {
        int cloud_size = TILE_SIDE*2;
        SDL_Rect spriteRect2 = {0, 0, cloud_size, cloud_size};
        SDL_Rect destRect2 = {(WIDTH-cloud_size)/2, cloud_size/2, cloud_size, cloud_size};
        SDL_RenderCopy(renderer, textures[CLOUD], &spriteRect2, &destRect2);

        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
        SDL_Rect destRect = {(WIDTH-TILE_SIDE)/2, cloud_size-TILE_SIDE/2, TILE_SIDE, TILE_SIDE};
        SDL_RenderCopy(renderer, textures[POWER_START+data.player.power], &spriteRect, &destRect);
    }

    if (data.ai.skin != -1 && data.ai.power != 0 && !(data.ai.power_time < power_duration*60/5 && (data.ai.power_time%10) < 5)) {
        int cloud_size = TILE_SIDE*2;
        SDL_Rect spriteRect2 = {0, 0, cloud_size, cloud_size};
        SDL_Rect destRect2 = {(WIDTH-cloud_size)/2 + 2*cloud_size, cloud_size/2, cloud_size, cloud_size};
        SDL_RenderCopy(renderer, textures[CLOUD], &spriteRect2, &destRect2);

        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
        SDL_Rect destRect = {(WIDTH-TILE_SIDE)/2 + 2 * cloud_size, cloud_size-TILE_SIDE/2, TILE_SIDE, TILE_SIDE};
        SDL_RenderCopy(renderer, textures[POWER_START+data.ai.power], &spriteRect, &destRect);
    }
}
