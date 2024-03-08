#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H

#include <SDL2/SDL.h>

class Pacman;
class Board;

class Ghost {
    // TODO: set random point that Ghost travel to
  public:
    static constexpr int height = 30;
    static constexpr int width = 30;
    static constexpr int velocity = 2;

    Ghost() : texture({0, 0, width, height}), velocity_x{0}, velocity_y{0} {}
    Ghost(int x, int y)
        : texture({x, y, width, height}), velocity_x{0}, velocity_y{0} {}

    void move(Board &b, Pacman &pacman);
    void render() const;

  private:
    // TODO: add sprite
    SDL_Rect texture;

    int velocity_x;
    int velocity_y;
};

#endif // !PACMAN_GHOST_H
