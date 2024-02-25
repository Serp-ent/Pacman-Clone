#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <cstdio>
#include <iostream>
#include <memory>

constexpr int screen_width = 800;
constexpr int screen_height = 600;

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

bool init_game() {
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("SDL could not initialize! SDL Error: %s\n",
                    SDL_GetError());
        return false;
    }
    gWindow = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, screen_width,
                               screen_height, SDL_WINDOW_SHOWN);
    if (!gWindow) {
        std::printf("SDL could not create window! SDL Error: %s\n",
                    SDL_GetError());
        return false;
    }

    // create renderer for window
    // TODO: check if we want to use VSYNC or just cap FPS
    gRenderer = SDL_CreateRenderer(
        gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!gRenderer) {
        std::printf("Renderer could not be created! SDL Error: %s\n",
                    SDL_GetError());
        return false;
    }

    // Initialize renderer color
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Create window

    return success;
}

void close_game() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
}

int main() {
    if (!init_game()) {
        std::printf("Failed to initialize!\n");
        return 1;
    }

    std::printf("Successfuly set up game\n");
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // clear screen
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        // update screen
        SDL_RenderPresent(gRenderer);
    }

    std::printf("Cleaning up game\n");

    close_game();
}
