#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H

#include "Behaviors.h"
#include "Ghost.h"
#include "LTexture.h"
#include "Timer.h"
#include <SDL2/SDL.h>
#include <memory>

class Board;

// TODO: Maybe use observer pattern
class Pacman {
  public:
    // TODO: Default height should be 16px and it should be multiplied by scale
    static constexpr int height = 30;
    static constexpr int width = 30;
    static constexpr int velocity = 2;

    // TODO: create sprite class
    static LTexture sprite;

    static constexpr int frames = 2;
    static SDL_Rect spriteClips[frames];
    static constexpr int deathFrames = 11;
    static SDL_Rect deathSpriteClips[deathFrames];
    static SDL_Rect notStartedClip;

    Pacman(int &points)
        : points{&points}, texture({0, 0, width, height}), isDead(false),
          velocity_x{0}, velocity_y{0} {}
    Pacman(int &points, int x, int y)
        : points{&points}, texture({x, y, width, height}), isDead(false),
          velocity_x{0}, velocity_y{0} {}

    void handleEvent(SDL_Event &e);
    void render();

    void move(Board &b, Ghost &g);

    const SDL_Rect &getCollision() const { return texture; }

    bool wasKilled() const { return isDead; }
    void clearState(bool death = false) { isDead = death; }

    bool playsDeathAnimation() { return playsAnimation; }

    int getLifesLeft() const { return livesLeft; }
    void setLifesLeft(int lives) { livesLeft = lives; }

    // start means user presses key to start moving pacman
    void start() { started = true; }
    // stop means wait for user keypress
    void stop() { started = false; }
    bool isStarted() { return started; }

  private:
    friend class PacmanDefaultBehavior;
    friend class PacmanSuperPointBehavior;
    std::unique_ptr<Behavior> behavior{new PacmanDefaultBehavior(*this)};

    int *points = nullptr;
    // TODO: add sprite
    SDL_Rect texture;
    SDL_Rect *currRect = &spriteClips[0];
    int angle_of_ratation = 0;

    bool isDead = false;
    bool playsAnimation = false;
    int livesLeft = 3;
    bool started = false;

    Timer attackerTime;

    int velocity_x;
    int velocity_y;
};

#endif // !PACMAN_PACMAN_H
