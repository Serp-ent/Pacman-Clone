#include "Box.h"
#include "Game.h"
#include "SDL2/SDL.h"

void Box::render(int x, int y) {
    SDL_Rect r{x, y, size, size};

    SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
    SDL_RenderFillRect(Game::gRenderer, &r);

    switch (type_) {
    case Type::super_point:
        SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0, 0, 0xFF);
        break;
    case Type::wall:
        SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        break;
    case Type::point:
        SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0xFF, 0, 0xFF);
        break;
    case Type::empty:
        SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xF);
        break;
    }

    r.w = r.h = size / 3;
    r.x += r.w;
    r.y += r.h;

    SDL_RenderFillRect(Game::gRenderer, &r);
};
