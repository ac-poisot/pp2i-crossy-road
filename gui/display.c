#include "display.h"

void display_text(char* text, int x, int y, int size, SDL_Renderer* renderer, SDL_Color color, char* font) {
    
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
        SDL_RenderCopy(renderer, textures[lane->type], &spriteRect, &destRect);
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
                SDL_RenderCopyEx(renderer, textures[TRAIN_EDGE], &spriteRect, &destRect, 0.0, NULL, flip);
                for (int i=1; i<(current_obstacle->size); i++) {
                    SDL_Rect destRect2 = {(current_obstacle->x+i)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                    SDL_RenderCopy(renderer, textures[TRAIN_MID], &spriteRect, &destRect2);
                }
                SDL_Rect destRect3 = {(current_obstacle->x+current_obstacle->size-1)*TILE_SIDE, screen_y*TILE_SIDE, TILE_SIDE, TILE_SIDE};
                SDL_RenderCopy(renderer, textures[TRAIN_EDGE], &spriteRect, &destRect3);

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
    SDL_Rect destRect = {player.x*TILE_SIDE, (cameraY- player.y)*TILE_SIDE, TILE_SIDE, TILE_SIDE};
    if (skin_set == 0) {
        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
        SDL_RenderCopyEx(renderer, textures[player.skin+SKIN_START], &spriteRect, &destRect, player.orientation, NULL, false);
    } else {
        SDL_Rect spriteRect = {TILE_SIDE*(((player.orientation+180)/90)%4), 0, TILE_SIDE, TILE_SIDE};
        SDL_RenderCopy(renderer, textures[player.skin+SKIN_START], &spriteRect, &destRect);
    }
}

void display(displayedData data, SDL_Renderer* renderer, SDL_Texture** textures, int skin_set) {
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

        // Display score
        char scoreText[20];
        SDL_Color white = {255, 255, 255, 255};
        sprintf(scoreText, "Score: %d", (int) data.player.y);
        display_text(scoreText, 0, 0, 24, renderer, white, "arial.ttf");
    }

    if (data.player.power != 0) {
        int cloud_size = TILE_SIDE*2;
        SDL_Rect spriteRect2 = {0, 0, cloud_size, cloud_size};
        SDL_Rect destRect2 = {(WIDTH-cloud_size)/2, cloud_size/2, cloud_size, cloud_size};
        SDL_RenderCopy(renderer, textures[CLOUD], &spriteRect2, &destRect2);


        SDL_Rect spriteRect = {0, 0, TILE_SIDE, TILE_SIDE};
        SDL_Rect destRect = {(WIDTH-TILE_SIDE)/2, cloud_size-TILE_SIDE/2, TILE_SIDE, TILE_SIDE};
        SDL_RenderCopy(renderer, textures[POWER_START+data.player.power], &spriteRect, &destRect);
    }
}
