#include "Ghost.h"

#include "Behaviors.h"
#include "Board.h"
#include <SDL2/SDL_render.h>
#include <cstdio>

SDL_Rect Ghost::spriteClips[Ghost::frames];
SDL_Rect Ghost::runningAwayClips[Ghost::runningAwayFrames];

void Ghost::move(Board &b, Entity &pacman) { behavior->move(b, pacman); }
void Ghost::render() {
    // // more rendering
    // SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0, 0, 0xAA);
    // SDL_RenderFillRect(Game::gRenderer, &texture);

    // SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
    // SDL_RenderDrawRect(Game::gRenderer, &texture);
    static int frame = 0;

    // TODO: frame speed scale
    // instead of magic number;
    if (wasKilled()) {
        // TODO: change sprite when returning to base
        SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0xFF, 0xFF, 100);
        SDL_RenderFillRect(Game::gRenderer, &texture);

        SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderDrawRect(Game::gRenderer, &texture);
        return;
    }

    ++frame;
    if (attacker) {
        // each direciton have only 2 sprites
        if (frame / 5 >= 2) {
            frame = 0;
        }

        currRect = &Ghost::spriteClips[directionSprite + frame / 5];
    } else {
        if (isBlinking()) {
            if (frame / 8 >= Ghost::runningAwayFrames) {
                frame = 0;
            }
        } else {
            if (frame / 8 >= (Ghost::runningAwayFrames / 2)) {
                frame = 0;
            }
        }
        currRect = &Ghost::runningAwayClips[frame / 8];
    }

    Pacman::sprite.render(texture.x, texture.y, *currRect, texture);
}

void Ghost::setAttack(bool attack) {
    Entity::setAttack(attack);

    // cast to prevent many memory allocations
    // TODO: find better way
    if (attacker) {
        RedGhostBehavior *b = dynamic_cast<RedGhostBehavior *>(behavior.get());
        if (!b) {
            behavior.reset(new RedGhostBehavior{*this});
        } else {
            b->resetPath();
        }
    } else {
        GhostRunAwayBehavior *b =
            dynamic_cast<GhostRunAwayBehavior *>(behavior.get());
        if (!b) {
            behavior.reset(new GhostRunAwayBehavior{*this});
        }
    }
}

void Ghost::clearState(bool death) {
    Entity::clearState(death);
    // if (isDead) {
    //     GhostDeathBehavior *b =
    //         dynamic_cast<GhostDeathBehavior *>(behavior.get());
    //     if (!b) {
    //         behavior.reset(new GhostDeathBehavior{*this});
    //     }
    // } else {
    //     RedGhostBehavior *b = dynamic_cast<RedGhostBehavior
    //     *>(behavior.get()); if (!b) {
    //         behavior.reset(new RedGhostBehavior{*this});
    //     }
    // }
}
