#include "Pacman.h"

#include "Board.h"
#include "utils.h"

#include "Game.h"
#include <SDL2/SDL_rect.h>

LTexture Pacman::sprite{};
SDL_Rect Pacman::spriteClips[Pacman::frames];
SDL_Rect Pacman::deathSpriteClips[Pacman::deathFrames];

void Pacman::move(Board &b) {
    SDL_Rect border{b.getPos().x, b.getPos().y, b.columns() * Box::size,
                    b.rows() * Box::size};

    // TODO: fix jumping
    // TODO: fix moving inside on rectnagle when there is border
    // e.g. pacman can move up and down when there are upper and lower boxes
    // // maybe keep surrounding 9 box that surrounds pacman
    // and check if we can enter them
    SDL_Point texture_center = {texture.x + Pacman::width / 2,
                                texture.y + Pacman::height / 2};

    int i = (texture_center.x - b.getPos().x) / Box::size;
    int j = (texture_center.y - b.getPos().y) / Box::size;
    if (velocity_x) {
        texture.x += velocity_x;
        // fix y position
        SDL_Point curr_box_center = {
            b.getPos().x + i * Box::size + Box::size / 2,
            b.getPos().y + j * Box::size + Box::size / 2};
        int correct_y = curr_box_center.y - texture.h / 2;
        texture.y = correct_y;

        if ((texture.x < border.x) ||
            (texture.x + texture.w > border.x + border.w) ||
            checkCollision(texture, b, Box::Type::wall)) {
            texture.x -= velocity_x;
        }
    } else if (velocity_y) {
        texture.y += velocity_y;

        // fix x position
        SDL_Point curr_box_center = {
            b.getPos().x + i * Box::size + Box::size / 2,
            b.getPos().y + j * Box::size + Box::size / 2};
        int correct_x = curr_box_center.x - texture.w / 2;
        texture.x = correct_x;

        if ((texture.y < border.y) ||
            (texture.y + texture.h) > border.y + border.h ||
            checkCollision(texture, b, Box::Type::wall)) {
            texture.y -= velocity_y;
        }
    }

    Box *box = nullptr;
    if (((box = pointIsReached(texture, b, Box::Type::point)) != nullptr) ||
        (box = pointIsReached(texture, b, Box::Type::super_point)) != nullptr) {

        ++*points;
        // TODO: emmit sound for eating ball point
        box->setType(Box::Type::empty);
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

void Pacman::render() {
    static int frame = 0;
    static int deathframe = 0;

    // TODO: frame speed scale
    // instead of magic number;

    ++frame;

    if (!isDead) {
        if (frame / 5 >= Pacman::frames) {
            frame = 0;
        }
        currRect = &Pacman::spriteClips[frame / 5];

        sprite.render(texture.x, texture.y, *currRect, texture,
                      angle_of_ratation);
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
