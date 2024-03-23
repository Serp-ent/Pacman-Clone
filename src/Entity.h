#ifndef PACMAN_ENTITY_H
#define PACMAN_ENTITY_H
#include "SDL2/SDL.h"
#include <SDL2/SDL_rect.h>
#include <memory>

class Board;
class Behavior;

class Entity {
  public:
    static constexpr int height = 30;
    static constexpr int width = 30;
    // default velocity
    // TODO: change name to speed
    static constexpr int velocity = 2;

    virtual ~Entity() {}
    virtual void move(Board &b, Entity &e) = 0;
    virtual void render() = 0;

    virtual const SDL_Rect &getCollision() const { return texture; }

    SDL_Point getPos() const {
        SDL_Point p{texture.x, texture.y};
        return p;
    }
    void setPos(const SDL_Point &p) {
        texture.x = p.x;
        texture.y = p.y;
    }

    bool wasKilled() const { return isDead; }
    virtual void clearState(bool death = false) { isDead = death; }

    virtual void setAttack(bool attack = true) { attacker = attack; }

    bool getAttack() const { return attacker; }

    virtual void reset(const SDL_Point &newPos) = 0;

  protected:
    // TODO: Rename to collision
    SDL_Rect texture{0, 0, width, height};
    SDL_Rect *currRect = nullptr;
    std::unique_ptr<Behavior> behavior;

    bool isDead = false;

    int velocity_x = 0;
    int velocity_y = 0;

    int attacker;
};

#endif // !PACMAN_ENTITY_H
