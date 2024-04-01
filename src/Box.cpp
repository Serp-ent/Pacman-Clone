#include "Box.h"
#include "Game.h"
#include "SDL2/SDL.h"

#include "Board.h"
#include <SDL2/SDL_render.h>

void Box::render(int x, int y) {
    SDL_Rect r{x, y, size, size};

    SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
    SDL_RenderFillRect(Game::gRenderer, &r);
    if (currClip) {
        Board::mapTexture.render(x, y, *currClip, r);

        return; // draw current texture and return
    }

    switch (type_) {
    case Type::super_point:
        r.w = r.h = size / 2;
        r.x += size / 2 - r.w / 2;
        r.y += size / 2 - r.h / 2;
        SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0, 0, 0xFF);
        break;
    case Type::wall:
        SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 100, 255);
        break;
    case Type::point:
        r.w = r.h = size / 6;
        r.x += size / 2 - r.w / 2;
        r.y += size / 2 - r.h / 2;
        SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0xFF, 0, 0xFF);
        break;
    case Type::empty:
        SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xF);
        break;
    case Type::ghostExit:
        SDL_SetRenderDrawColor(Game::gRenderer, 0, 100, 0, 0xF);
        break;
    }

    SDL_RenderFillRect(Game::gRenderer, &r);
};

Box::WallType Box::getClip() {
    return static_cast<Box::WallType>(currClip - Board::mapClips);
}
