#include "button.h"

void display_button(button b, SDL_Renderer* renderer, SDL_Texture** textures) {
    SDL_Rect spriteRect = {0, 0, b.width, b.height};
    SDL_Rect destRect = {b.x, b.y, b.width, b.height};
    SDL_RenderCopy(renderer, textures[b.texture], &spriteRect, &destRect);
}

bool button_clicked(button b, SDL_Event event) {
    return event.button.x > b.x
        && event.button.x <= b.x + b.width
        && event.button.y > b.y
        && event.button.y <= b.y + b.height;
}
