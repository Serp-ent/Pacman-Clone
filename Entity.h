#ifndef PACMAN_ENTITY_H
#define PACMAN_ENTITY_H
#include "SDL2/SDL.h"
#include <memory>

class Board;
class Behavior;

class Entity {
  public:
    static constexpr int height = 30;
    static constexpr int width = 30;
    // default velocity
    static constexpr int velocity = 2;

    virtual void move(Board &b, Entity &e) = 0;
    virtual void render() = 0;

    virtual const SDL_Rect &getCollision() const { return texture; }

    // TODO: add get pos

    bool wasKilled() const { return isDead; }
    void clearState(bool death = false) { isDead = death; }

  public:
    SDL_Rect texture{0, 0, width, height};
    SDL_Rect *currRect = nullptr;
    std::unique_ptr<Behavior> behavior;

    bool isDead = false;

    int velocity_x = 0;
    int velocity_y = 0;
};

#endif // !PACMAN_ENTITY_H
