#include "Hud.h"

#include "Game.h"
#include "namedColors.h"
#include <SDL2/SDL_rect.h>
#include <sstream>

void Hud::refresh() {
    std::ostringstream oss;
    oss << "Points: " << points;
    points_texture.loadText(oss.str(), white);

    oss.str("");
    oss << "FPS: " << fps;
    fps_texture.loadText(oss.str(), white);
}

void Hud::render() {
    this->refresh();

    // TODO: 10 is padding (magic constant)
    points_texture.render(10, Game::screen_height - points_texture.getHeight() -
                                  10);
    fps_texture.render(Game::screen_width - fps_texture.getWidth() - 10, 10);

    SDL_Rect dest_size{Game::screen_width - (Pacman::height * livesLeft) - 10,
                       Game::screen_height - (Pacman::width)-10, Pacman::width,
                       Pacman::height};
    for (int i = 0; i < livesLeft; ++i) {
        Pacman::sprite.render(dest_size.x, dest_size.y, Pacman::spriteClips[0],
                              dest_size);
        dest_size.x += Pacman::width;
    }
}

void Hud::gamePaused() {
    SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(Game::gRenderer);
    paused_texture.render(
        Game::screen_width / 2 - points_texture.getWidth() / 2,
        Game::screen_height / 2 - points_texture.getHeight() / 2);

    SDL_RenderPresent(Game::gRenderer);
}
