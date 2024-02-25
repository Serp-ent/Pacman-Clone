#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <cstdio>
#include <iostream>
#include <memory>

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

constexpr int screen_width = 800;
constexpr int screen_height = 600;

class Pacman {
  public:
    static constexpr int height = 40;
    static constexpr int width = 40;
    static constexpr int velocity = 1;

    Pacman() : texture({0, 0, width, height}), velocity_x{0}, velocity_y{0} {}

    void handleEvent(SDL_Event &e);
    void move();
    void render() const;

  private:
    // TODO: add sprite
    SDL_Rect texture;

    int velocity_x;
    int velocity_y;
};

void Pacman::move() {
    texture.x += velocity_x;
    if ((texture.x < 0) || (texture.x + texture.w > screen_width)) {
        texture.x -= velocity_x;
    }

    texture.y += velocity_y;
    if ((texture.y < 0) || (texture.y + texture.h) > screen_height) {
        texture.y -= velocity_y;
    }
}

void Pacman::handleEvent(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
            velocity_y -= velocity;
            break;
        case SDLK_DOWN:
            velocity_y += velocity;
            break;
        case SDLK_RIGHT:
            velocity_x += velocity;
            break;
        case SDLK_LEFT:
            velocity_x -= velocity;
            break;
        }
    } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
            velocity_y += velocity;
            break;
        case SDLK_DOWN:
            velocity_y -= velocity;
            break;
        case SDLK_RIGHT:
            velocity_x -= velocity;
            break;
        case SDLK_LEFT:
            velocity_x += velocity;
            break;
        }
    }
}

void Pacman::render() const {
    // more rendering
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(gRenderer, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0, 0xFF);
    SDL_RenderFillRect(gRenderer, &texture);

    SDL_SetRenderDrawColor(gRenderer, r, g, b, a);
}

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
    Pacman pacman;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            pacman.handleEvent(e);
        }
        pacman.move();

        // clear screen
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        pacman.render();

        // update screen
        SDL_RenderPresent(gRenderer);
    }

    std::printf("Cleaning up game\n");

    close_game();
}
