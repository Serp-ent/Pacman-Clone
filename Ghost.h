#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H

#include "Behaviors.h"
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

    Ghost() {
        currRect = &spriteClips[directionSprite];
        attacker = true;
        behavior.reset(new DumpGhostBehavior(*this));
    }
    Ghost(int x, int y) : Ghost{} {
        texture.x = x;
        texture.y = y;
    }

    virtual void move(Board &b, Entity &e) override;
    virtual void render() override;

  private:
    friend class DumpGhostBehavior;

    // TODO: add sprite
    int directionSprite = 0;
};

#endif // !PACMAN_GHOST_H
