#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H

#include "Behaviors.h"
#include "Entity.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <array>
#include <memory>

class Pacman;
class Board;

class Ghost : public Entity {
  public:
    // TODO: clips should be in one array accessed by macros for each state
    enum Type {
        red,    // Blinky
        pink,   // Pinky
        cyan,   // Inky
        orange, // Clyde
        ghostNumber,
    };

    static constexpr int frames = 8;
    static constexpr int runningAwayFrames = 4;

    using GhostClips = std::array<SDL_Rect, frames>;

    static std::array<GhostClips, Type::ghostNumber> spriteClips;

    static SDL_Rect runningAwayClips[runningAwayFrames];

    Ghost(Type t) : ghostColor(t) {
        currRect = &spriteClips[ghostColor][directionSprite];
        attacker = true;

        behavior = getGhostBehavior(*this);
    }
    Ghost(Type t, const SDL_Point &pos) : Ghost{t} {
        texture.x = pos.x;
        texture.y = pos.y;
    }

    static std::unique_ptr<Behavior> getGhostBehavior(Ghost &ghost);

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

    virtual void reset(const SDL_Point &newPos) override {
        clearState();
        velocity_x = velocity_y = 0;
        setPos(newPos);

        setAttack(true);
        blink = false;
    }

    void setBlinking(bool b = true) { blink = b; }
    bool isBlinking() { return blink; }

  private:
    friend class DumbGhostBehavior;
    friend class GhostDeathBehavior;
    friend class RedGhostBehavior;
    friend class GhostRunAwayBehavior;

    // TODO: add sprite
    int directionSprite = 0;
    bool blink = false;
    const Type ghostColor;
};

#endif // !PACMAN_GHOST_H
