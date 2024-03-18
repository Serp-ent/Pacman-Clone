#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H

#include "Behaviors.h"
#include "Entity.h"
#include <SDL2/SDL.h>

class Pacman;
class Board;

class Ghost : public Entity {
  public:
    // TODO: clips should be in one array accessed by macros for each state
    static constexpr int frames = 10;
    static SDL_Rect spriteClips[frames];
    static constexpr int runningAwayFrames = 2;
    static SDL_Rect runningAwayClips[runningAwayFrames];

    Ghost() {
        currRect = &spriteClips[directionSprite];
        attacker = true;
        behavior.reset(new RedGhostBehavior(*this));
    }
    Ghost(int x, int y) : Ghost{} {
        texture.x = x;
        texture.y = y;
    }

    virtual void move(Board &b, Entity &e) override;
    virtual void render() override;

    void die(Board &b) {
        Entity::clearState(true);

        GhostDeathBehavior *newBehavior = new GhostDeathBehavior{*this};
        newBehavior->loadPathToHome(b);

        behavior.reset(newBehavior);
    }
    virtual void clearState(bool death = false) override;
    virtual void setAttack(bool attack = true) override;

  private:
    friend class DumbGhostBehavior;
    friend class GhostDeathBehavior;
    friend class RedGhostBehavior;
    friend class GhostRunAwayBehavior;

    // TODO: add sprite
    int directionSprite = 0;
};

#endif // !PACMAN_GHOST_H