#include "Ghost.h"

#include "Board.h"
#include "utils.h"
#include <SDL2/SDL_render.h>
#include "Game.h"

void Ghost::render() const {
    // more rendering
    SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0, 0, 0xAA);
    SDL_RenderFillRect(Game::gRenderer, &texture);

    SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
    SDL_RenderDrawRect(Game::gRenderer, &texture);
}

void Ghost::move(Board &b, Pacman &pacman) {

    // TODO: non-random decisions
    static int moveNumber = 0;
    if (moveNumber == 40) {
        int direction = random();
        // TODO: velocity 
        velocity_y = 0;
        velocity_x = 0;
        switch (direction % 4) {
        case 0:
            velocity_x += 2;
            break;
        case 1:
            velocity_x -= 2;
            break;
        case 2:
            velocity_y -= 2;
            break;
        case 3:
            velocity_y += 2;
            break;
        }

        moveNumber = 0;
    }

    ++moveNumber;

    SDL_Rect border{b.getPos().x, b.getPos().y, b.columns() * Box::size,
                    b.rows() * Box::size};

    // TODO: fix jumping
    // TODO: fix moving inside on rectnagle when there is border
    // e.g. can move up and down when there are upper and lower boxes
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

    // Make collision only point so textures can overlap
    SDL_Point collison{texture.x + texture.w / 2, texture.y + texture.h / 2};
    if (SDL_PointInRect(&collison, &pacman.getCollision())) {
        pacman.clearState(true);
    }
}
