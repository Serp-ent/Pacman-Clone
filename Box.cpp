#include "Box.h"
#include "SDL2/SDL.h"

// HACK: maybe use a common header
extern SDL_Renderer *gRenderer;

void Box::render(int x, int y) {
    SDL_Rect r{x, y, size, size};

    switch (type_) {
    case Type::super_point:
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0xFF);
        break;
    case Type::wall:
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
        break;
    case Type::point:
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0, 0xFF);
        break;
    case Type::empty:
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xF);
        break;
    }

    SDL_RenderFillRect(gRenderer, &r);
};
