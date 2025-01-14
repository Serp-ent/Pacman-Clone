#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H

#include "Behaviors.h"
#include "Entity.h"
#include "Ghost.h"
#include "LTexture.h"
#include "Timer.h"
#include <SDL2/SDL.h>
#include <memory>

class Board;

// TODO: Maybe use observer pattern
class Pacman : public Entity {
  public:
    // TODO: Default height should be 16px and it should be multiplied by scale
    // TODO: create sprite class

    // TODO: move sprite to entity abstract class
    static LTexture sprite;

    virtual void move(Board &b, Entity &e) override;
    virtual void render() override;

    static constexpr int frames = 2;
    static SDL_Rect spriteClips[frames];
    static constexpr int deathFrames = 11;
    static SDL_Rect deathSpriteClips[deathFrames];
    static SDL_Rect notStartedClip;

    Pacman(int &points) : points{&points} {
        currRect = &notStartedClip;
        behavior.reset(new PacmanDefaultBehavior{*this});
        attacker = false;
    }
    Pacman(int &points, const SDL_Point &pos) : Pacman{points} {
        texture.x = pos.x;
        texture.y = pos.y;
    }

    void handleEvent(SDL_Event &e);

    bool playsDeathAnimation() { return playsAnimation; }

    int getLifesLeft() const { return livesLeft; }
    void setLifesLeft(int lives) {
        livesLeft = lives;
        if (livesLeft > 0) {
            clearState(); // make him alive
        }
    }

    virtual void reset(const SDL_Point &newPos) override {
        attackerTime.stop();
        playsAnimation = false;
        started = false;

        setAttack(false);
        setPos(newPos);
        clearState();

        velocity_x = velocity_y = 0;
    }

    // start means user presses key to start moving pacman
    void start() { started = true; }
    // stop means wait for user keypress
    void stop() { started = false; }
    bool isStarted() { return started; }

  private:
    friend class PacmanDefaultBehavior;
    friend class PacmanSuperPointBehavior;

    int *points = nullptr;
    // TODO: add sprite
    int angle_of_ratation = 0;

    bool playsAnimation = false;

    int livesLeft = 3;
    bool started = false;

    Timer attackerTime;
};

#endif // !PACMAN_PACMAN_H
