#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H

#include "Entity.h"
#include <SDL2/SDL.h>

class Pacman;
class Board;

// TODO: ghost should be abstract class
// and there each ghost should have different level of aggression (different
// algorithm) and differnet behavior (maybe strategy pattern instead of
// inheritance)
class Ghost : public Entity {
    // TODO: set random point that Ghost travel to
  public:
    // TODO: clips should be in one array accessed by macros for each state
    static constexpr int frames = 10;
    static SDL_Rect spriteClips[frames];
    static constexpr int runningAwayFrames = 2;
    static SDL_Rect runningAwayClips[runningAwayFrames];

    Ghost() { currRect = &spriteClips[directionSprite]; }
    Ghost(int x, int y) : Ghost{} {
        texture.x = x;
        texture.y = y;
        // TODO: remove if not needed (don't believe ghost{} to invoke default
        // ctor)
        currRect = &spriteClips[directionSprite];
    }

    virtual void move(Board &b, Entity &e) override;
    virtual void render() override;

    void setAttack(bool attack = true) { attacker = attack; }
    bool getAttack() const { return attacker; }

  private:
    // TODO: add sprite
    int directionSprite = 0;
    int attacker = true;
};

#endif // !PACMAN_GHOST_H
