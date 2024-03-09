#include "Hud.h"

#include "Game.h"
#include "namedColors.h"
#include <sstream>

void Hud::refresh() {
    std::ostringstream oss;
    oss << "Points: " << points;
    points_texture.loadText(oss.str(), white);

    oss.str("");
    oss << "FPS: " << fps;
    fps_texture.loadText(oss.str(), white);

    oss.str("");
    oss << "Lives: " << livesLeft;
    livesLeft_texture.loadText(oss.str(), white);
}

void Hud::render() {
    this->refresh();

    points_texture.render(10, Game::screen_height - points_texture.getHeight() -
                                  10);
    fps_texture.render(Game::screen_width - fps_texture.getWidth() - 10, 10);
    livesLeft_texture.render(
        Game::screen_width - livesLeft_texture.getWidth() - 10,
        Game::screen_height - livesLeft_texture.getHeight() - 10);
}
