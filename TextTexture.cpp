#include "TextTexture.h"

#include <SDL2/SDL_ttf.h>

// HACK: use common header
extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;

void TextTexture::free() {
    if (!texture) {
        return;
    }

    SDL_DestroyTexture(texture);
    texture = nullptr;
    width = height = 0;
}
void TextTexture::set_color(Uint8 r, Uint8 g, Uint8 b) {}

void TextTexture::render(int x, int y) {
    SDL_Rect render = {x, y, width, height};
    SDL_RenderCopy(gRenderer, texture, NULL, &render);
}

bool TextTexture::loadText(const std::string &text, SDL_Color color) {
    // get rid of preexisintg texture
    free();

    SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, text.c_str(), color);
    if (!textSurface) {
        std::printf("Unable to render text surface! SDL_ttf Error: %s\n",
                    TTF_GetError());
        return false;
    }

    // create texture from surface pixels
    bool success = true;
    texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (!texture) {
        std::printf(
            "Unable to create texture from rendered text! SDL Error: %s\n",
            SDL_GetError());
        success = false;
    } else {
        // get image dimensions
        width = textSurface->w;
        height = textSurface->h;
    }

    SDL_FreeSurface(textSurface);

    return success;
}
