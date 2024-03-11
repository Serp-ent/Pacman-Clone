#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H

#include <SDL2/SDL.h>

class Pacman;
class Board;

// TODO: ghost should be abstract class
// and there each ghost should have different level of aggression (different
// algorithm) and differnet behavior (maybe strategy pattern instead of
// inheritance)
class Ghost {
    // TODO: set random point that Ghost travel to
  public:
    static constexpr int height = 30;
    static constexpr int width = 30;
    static constexpr int velocity = 2;

    // TODO: clips should be in one array accessed by macros for each state
    static constexpr int frames = 10;
    static SDL_Rect spriteClips[frames];
    static constexpr int runningAwayFrames = 2;
    static SDL_Rect runningAwayClips[runningAwayFrames];

    Ghost() : texture({0, 0, width, height}), velocity_x{0}, velocity_y{0} {}
    Ghost(int x, int y)
        : texture({x, y, width, height}), velocity_x{0}, velocity_y{0} {}

    void move(Board &b, Pacman &pacman);
    void render();

    void setAttack(bool attack = true) { attacker = attack; }
    bool getAttack() const { return attacker; }

  private:
    // TODO: add sprite
    SDL_Rect texture;                       // collision box
    SDL_Rect *currSprite = &spriteClips[0]; // collision box
    int directionSprite = 0;
    int attacker = true;

    int velocity_x;
    int velocity_y;
};

#endif // !PACMAN_GHOST_H
