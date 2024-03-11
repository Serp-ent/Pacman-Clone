#include "Pacman.h"

#include "Behaviors.h"
#include "Board.h"

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <cstdio>

LTexture Pacman::sprite{};
SDL_Rect Pacman::spriteClips[Pacman::frames];
SDL_Rect Pacman::deathSpriteClips[Pacman::deathFrames];
SDL_Rect Pacman::notStartedClip;

// TODO: behavior-> move should take entity as parameter
void Pacman::move(Board &b, Entity &ghost) {
    behavior->move(b, ghost);
    // TODO: some cast to prevent many memory allocations
    if (attacker) {
        behavior.reset(new PacmanSuperPointBehavior{*this});
    } else {
        behavior.reset(new PacmanDefaultBehavior{*this});
    }
}

void Pacman::render() {
    static int frame = 0;
    static int deathframe = 0;

    // TODO: frame speed scale
    // instead of magic number;

    ++frame;

    if (!isDead) {
        if (!isStarted()) {
            sprite.render(texture.x, texture.y, Pacman::notStartedClip, texture,
                          angle_of_ratation);
            return;
        }

        if (frame / 5 >= Pacman::frames) {
            frame = 0;
        }
        currRect = &Pacman::spriteClips[frame / 5];
        sprite.render(texture.x, texture.y, *currRect, texture,
                      angle_of_ratation);

        // TODO: move to function where we kill pacman
        deathframe = 0;
    } else {
        playsAnimation = true;
        ++deathframe;

        if (deathframe / 8 >= Pacman::deathFrames) {
            playsAnimation = false;
            deathframe = 0;
            return;
        }

        currRect = &Pacman::deathSpriteClips[deathframe / 8];

        sprite.render(texture.x, texture.y, *currRect, texture,
                      angle_of_ratation);
    }
}

void Pacman::handleEvent(SDL_Event &e) {
    // TODO: pacman should not change direction when there is collison
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
            velocity_y = -velocity;
            angle_of_ratation = 3 * 90;
            velocity_x = 0;
            break;
        case SDLK_DOWN:
            velocity_y = velocity;
            angle_of_ratation = 1 * 90;
            velocity_x = 0;
            break;
        case SDLK_RIGHT:
            angle_of_ratation = 0;
            velocity_y = 0;
            velocity_x = velocity;
            break;
        case SDLK_LEFT:
            angle_of_ratation = 2 * 90;
            velocity_y = 0;
            velocity_x = -velocity;
            break;
        }
    }
}
