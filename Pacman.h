#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H

#include <SDL2/SDL.h>

class Board;

class Pacman {
  public:
    static constexpr int height = 30;
    static constexpr int width = 30;
    static constexpr int velocity = 2;

    Pacman()
        : texture({0, 0, width, height}), isDead(false), velocity_x{0},
          velocity_y{0} {}
    Pacman(int x, int y)
        : texture({x, y, width, height}), isDead(false), velocity_x{0},
          velocity_y{0} {}

    void handleEvent(SDL_Event &e);
    void move(Board &b, int &points);
    void render() const;

    const SDL_Rect &getCollision() const { return texture; }

    bool wasKilled() const { return isDead; }
    void clearState(bool death = false) { isDead = death; }

    int getLifesLeft() const { return livesLeft; }
    void setLifesLeft(int lives) { livesLeft = lives; }

  private:
    // TODO: add sprite
    SDL_Rect texture;
    bool isDead = false;
    int livesLeft = 3;

    int velocity_x;
    int velocity_y;
};

#endif // !PACMAN_PACMAN_H
