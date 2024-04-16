#include "Ghost.h"

#include "Behaviors.h"
#include "Board.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <array>
#include <cstdio>

std::array<Ghost::GhostClips, Ghost::Type::ghostNumber> Ghost::spriteClips;
SDL_Rect Ghost::runningAwayClips[Ghost::runningAwayFrames];

void Ghost::move(Board &b, Entity &pacman) { behavior->move(b, pacman); }

void pickRenderDrawColor(Ghost::Type t) {
    switch (t) {
    case Ghost::Type::red:
        SDL_SetRenderDrawColor(Game::gRenderer, 255, 0, 0, 255);
        break;
    case Ghost::Type::cyan:
        SDL_SetRenderDrawColor(Game::gRenderer, 0, 255, 255, 255);
        break;
    case Ghost::Type::pink:
        SDL_SetRenderDrawColor(Game::gRenderer, 255, 192, 203, 255);
        break;
    case Ghost::Type::orange:
        SDL_SetRenderDrawColor(Game::gRenderer, 255, 165, 0, 255);
        break;
    case Ghost::ghostNumber:
        // pass
        break;
    }
}

void Ghost::renderPath(const SDL_Point &boardPos) {
    constexpr int pathBoxSize = 7;
    {
        GhostAttackBehavior *b =
            dynamic_cast<GhostAttackBehavior *>(behavior.get());
        if (b) {
            pickRenderDrawColor(ghostColor);
            for (auto p : b->getPath()) {
                // TODO: function that changes from Box cordinates to pixels
                // and from pixels to box coordinates
                SDL_Rect path{
                    boardPos.x + p->x * Box::size + Box::size / 2 - pathBoxSize / 2,
                    boardPos.y + p->y * Box::size + Box::size / 2 - pathBoxSize / 2, pathBoxSize,
                    pathBoxSize};
                SDL_RenderFillRect(Game::gRenderer, &path);
            }
            return;
        }
    }

    {
        GhostRunAwayBehavior *b =
            dynamic_cast<GhostRunAwayBehavior *>(behavior.get());
        if (b) {
            SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0xFF, 0xFF);
            for (auto p : b->getPath()) {
                // TODO: function that changes from Box cordinates to pixels
                // and from pixels to box coordinates
                SDL_Rect path{
                    boardPos.x + p->x * Box::size + Box::size / 2 - pathBoxSize / 2,
                    boardPos.y + p->y * Box::size + Box::size / 2 - pathBoxSize / 2, pathBoxSize,
                    pathBoxSize};
                SDL_RenderFillRect(Game::gRenderer, &path);
            }
            return;
        }
    }

    {
        GhostDeathBehavior *b =
            dynamic_cast<GhostDeathBehavior *>(behavior.get());
        if (b) {
            SDL_SetRenderDrawColor(Game::gRenderer, 255, 255, 255, 0xFF);
            for (auto p : b->getPath()) {
                // TODO: function that changes from Box cordinates to pixels
                // and from pixels to box coordinates
                SDL_Rect path{
                    boardPos.x + p->x * Box::size + Box::size / 2 - pathBoxSize / 2,
                    boardPos.y + p->y * Box::size + Box::size / 2 - pathBoxSize / 2, pathBoxSize,
                    pathBoxSize};
                SDL_RenderFillRect(Game::gRenderer, &path);
            }
            return;
        }
    }
}

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

        currRect = &Ghost::spriteClips[ghostColor][directionSprite + frame / 5];
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
        GhostAttackBehavior *b =
            dynamic_cast<GhostAttackBehavior *>(behavior.get());
        if (!b) {
            behavior = getGhostBehaviorColorBased(*this);
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

std::unique_ptr<Behavior> Ghost::getGhostBehaviorColorBased(Ghost &ghost) {
    std::unique_ptr<Behavior> behavior;
    switch (ghost.ghostColor) {
    case Type::red:
        behavior = std::make_unique<RedGhostBehavior>(ghost);
        break;
    case Type::pink:
        behavior = std::make_unique<PinkGhostBehavior>(ghost);
        break;
    case Type::cyan:
        behavior = std::make_unique<CyanGhostBehavior>(ghost);
        break;
    case Type::orange:
        behavior = std::make_unique<OrangeGhostBehavior>(ghost);
        break;
    case Type::ghostNumber:
        printf("[ERROR] ghost number provided as argument\n");
        break;
    }

    return behavior;
}
