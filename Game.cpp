#include "Game.h"

// TODO: check if we can have unique_ptr provided method
// that closes in dtor
SDL_Window *Game::gWindow = nullptr;
SDL_Renderer *Game::gRenderer = nullptr;
TTF_Font *Game::gFont = nullptr;

Game::Game() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("SDL could not initialize! SDL Error: %s\n",
                    SDL_GetError());
        throw std::runtime_error{"SDL could not initialize"};
    }

    Game::gWindow = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, screen_width,
                                     screen_height, SDL_WINDOW_SHOWN);
    if (!Game::gWindow) {
        std::printf("SDL could not create window! SDL Error: %s\n",
                    SDL_GetError());
        throw std::runtime_error{"SDL could not create window"};
    }

    // create renderer for window
    // TODO: check if we want to use VSYNC or just cap FPS
    Game::gRenderer = SDL_CreateRenderer(Game::gWindow, -1,
                                         SDL_RENDERER_ACCELERATED |
                                             SDL_RENDERER_PRESENTVSYNC);
    if (!Game::gRenderer) {
        std::printf("Renderer could not be created! SDL Error: %s\n",
                    SDL_GetError());
        throw std::runtime_error("Render could not be created");
    }

    // Initialize renderer color
    SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                    TTF_GetError());
        throw std::runtime_error("SDL_ttf could not initialize");
    }

    load_media();

    std::printf("Successfuly set up game\n");
}

void Game::load_media() {
    Game::gFont = TTF_OpenFont("./Oswald-VariableFont_wght.ttf", Box::size / 2);
    if (!Game::gFont) {
        std::printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        throw std::runtime_error{"Failed to load font"};
    }
}

void Game::close_game() {
    TTF_CloseFont(Game::gFont);

    SDL_DestroyRenderer(Game::gRenderer);
    SDL_DestroyWindow(Game::gWindow);

    // quit subsystems
    TTF_Quit();
    SDL_Quit();

    std::printf("Cleaning up game\n");
}
