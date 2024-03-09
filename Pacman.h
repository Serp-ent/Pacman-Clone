#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H

#include "LTexture.h"
#include <SDL2/SDL.h>

class Board;

// TODO: Maybe use observer pattern
class Pacman {
  public:
    static constexpr int height = 30;
    static constexpr int width = 30;
    static constexpr int velocity = 2;

    // TODO: create sprite class
    static LTexture sprite;
    static constexpr int frames = 2;
    static SDL_Rect spriteClips[frames];

    Pacman(int &points)
        : points{&points}, texture({0, 0, width, height}), isDead(false),
          velocity_x{0}, velocity_y{0} {}
    Pacman(int &points, int x, int y)
        : points{&points}, texture({x, y, width, height}), isDead(false),
          velocity_x{0}, velocity_y{0} {}

    void handleEvent(SDL_Event &e);
    void move(Board &b);
    void render();

    const SDL_Rect &getCollision() const { return texture; }

    bool wasKilled() const { return isDead; }
    void clearState(bool death = false) { isDead = death; }

    int getLifesLeft() const { return livesLeft; }
    void setLifesLeft(int lives) { livesLeft = lives; }

  private:
    int *points = nullptr;
    // TODO: add sprite
    SDL_Rect texture;
    SDL_Rect *currRect = &spriteClips[0];
    int angle_of_ratation = 0;

    bool isDead = false;
    int livesLeft = 3;

    int velocity_x;
    int velocity_y;
};

#endif // !PACMAN_PACMAN_H
