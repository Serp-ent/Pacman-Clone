#include "Behaviors.h"

#include "Board.h"
#include "SDL2/SDL.h"
#include "utils.h"

void PacmanDefaultBehavior::move(Board &b) {
    SDL_Rect border{b.getPos().x, b.getPos().y, b.columns() * Box::size,
                    b.rows() * Box::size};

    // TODO: fix jumping
    // TODO: fix moving inside on rectnagle when there is border
    // e.g. pacman can move up and down when there are upper and lower boxes
    // // maybe keep surrounding 9 box that surrounds pacman
    // and check if we can enter them
    SDL_Point texture_center = {pacman.texture.x + Pacman::width / 2,
                                pacman.texture.y + Pacman::height / 2};

    if (pacman.attackerTime.getTicks() > 5'000) {
        pacman.attackerTime.stop();
        std::printf("Pacman is now chased\n");
        // TODO:
        // ghost.setAttack();
    }

    int i = (texture_center.x - b.getPos().x) / Box::size;
    int j = (texture_center.y - b.getPos().y) / Box::size;
    if (pacman.velocity_x) {
        pacman.texture.x += pacman.velocity_x;
        // fix y position
        SDL_Point curr_box_center = {
            b.getPos().x + i * Box::size + Box::size / 2,
            b.getPos().y + j * Box::size + Box::size / 2};
        int correct_y = curr_box_center.y - pacman.texture.h / 2;
        pacman.texture.y = correct_y;

        if ((pacman.texture.x < border.x) ||
            (pacman.texture.x + pacman.texture.w > border.x + border.w) ||
            checkCollision(pacman.texture, b, Box::Type::wall)) {
            pacman.texture.x -= pacman.velocity_x;
        }
    } else if (pacman.velocity_y) {
        pacman.texture.y += pacman.velocity_y;

        // fix x position
        SDL_Point curr_box_center = {
            b.getPos().x + i * Box::size + Box::size / 2,
            b.getPos().y + j * Box::size + Box::size / 2};
        int correct_x = curr_box_center.x - pacman.texture.w / 2;
        pacman.texture.x = correct_x;

        if ((pacman.texture.y < border.y) ||
            (pacman.texture.y + pacman.texture.h) > border.y + border.h ||
            checkCollision(pacman.texture, b, Box::Type::wall)) {
            pacman.texture.y -= pacman.velocity_y;
        }
    }

    Box *box = pointIsReached(pacman.texture, b, Box::Type::point);
    if (box != nullptr) {
        ++*pacman.points;
        // change pacman move behavior for 5seconds (now he is attacker)
        // change ghost move behavior for 5seconds (now they are running away)
        // TODO: emmit sound for eating ball point
        box->setType(Box::Type::empty);
        return;
    }

    box = pointIsReached(pacman.texture, b, Box::Type::super_point);
    if (box != nullptr) {
        std::printf("Super point reached\n");
        // TODO:
        // ghost.setAttack(false);
        pacman.attackerTime.start();
        ++(*pacman.points);
        // TODO: emmit sound for eating ball point
        box->setType(Box::Type::empty);
    }
}
