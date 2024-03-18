#include "LTexture.h"
#include "Game.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

LTexture::LTexture() {
    // Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture() {
    // Deallocate
    free();
}

bool LTexture::loadFromFile(const std::string &path) {
    // Get rid of preexisting texture
    free();
    // The final texture
    SDL_Texture *newTexture = NULL;

    // Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
               IMG_GetError());
    } else {
        // Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE,
                        SDL_MapRGB(loadedSurface->format, 0, 0, 0));
        // Create texture from surface pixels
        newTexture =
            SDL_CreateTextureFromSurface(Game::gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n",
                   path.c_str(), SDL_GetError());
        } else {
            // Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    // Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

void LTexture::free() {
    // Free texture if it exists
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y) {
    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    SDL_RenderCopy(Game::gRenderer, mTexture, NULL, &renderQuad);
}

void LTexture::render(int x, int y, SDL_Rect &srcRect, SDL_Rect &destRect) {
    // Render to screen
    SDL_RenderCopy(Game::gRenderer, mTexture, &srcRect, &destRect);
}

void LTexture::render(int x, int y, SDL_Rect &srcRect, SDL_Rect &destRect,
                      double angle) {
    // Render to screen
    SDL_RenderCopyEx(Game::gRenderer, mTexture, &srcRect, &destRect, angle,
                     nullptr, SDL_FLIP_NONE);
}

void LTexture::render(int x, int y, SDL_Rect *clip) {
    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    // Set clip rendering dimensions
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    // Render to screen
    SDL_RenderCopy(Game::gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth() { return mWidth; }

int LTexture::getHeight() { return mHeight; }
