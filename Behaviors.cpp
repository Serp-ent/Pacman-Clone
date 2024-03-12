#include "Behaviors.h"

#include "Board.h"
#include "SDL2/SDL.h"
#include "utils.h"
#include <cstdio>

void PacmanDefaultBehavior::move(Board &b, Entity &ghost) {
    // TODO: maybe check that entiy != pacman to prevent killing itself

    // TODO:
    // another way is to do dynamic_cast and exit with error that given
    // argument was wrong type (bad - no compile time error checking)

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
        ghost.setAttack();
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
        ghost.setAttack(false);
        pacman.attackerTime.start();
        pacman.setAttack(true);

        ++(*pacman.points);
        box->setType(Box::Type::empty);

        // TODO: emmit sound for eating ball point
    }
}

void PacmanSuperPointBehavior::move(Board &b, Entity &ghost) {
    // TODO: maybe check that entiy != pacman to prevent killing itself

    // TODO:
    // another way is to do dynamic_cast and exit with error that given
    // argument was wrong type (bad - no compile time error checking)

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
        ghost.setAttack();
        pacman.setAttack(false);
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
    // Make collision only point so textures can overlap
    SDL_Rect ghostCollision = ghost.getCollision(); // top left corner
    SDL_Point collison{ghostCollision.x + ghostCollision.w / 2,
                       ghostCollision.y + ghostCollision.h / 2};
    if (SDL_PointInRect(&collison, &pacman.getCollision())) {
        Ghost *g = dynamic_cast<Ghost *>(&ghost);
        g->die(b);
        // ghost.clearState(true);
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
        // TODO: create some function as switchRoles()
        ghost.setAttack(false);
        pacman.attackerTime.start();
        pacman.setAttack();

        ++(*pacman.points);
        box->setType(Box::Type::empty);

        // TODO: emmit sound for eating ball point
    }
}

void DumbGhostBehavior::move(Board &b, Entity &pacman) {
    // TODO: non-random decisions
    static int moveNumber = 0;
    if (moveNumber == 40) {
        int direction = random();
        // TODO: velocity
        // TODO: get rid of magic constants
        ghost.velocity_y = 0;
        ghost.velocity_x = 0;
        switch (direction % 4) {
        case 0:
            ghost.directionSprite = 0;
            ghost.velocity_x += 2;
            break;
        case 1:
            ghost.directionSprite = 2;
            ghost.velocity_x -= 2;
            break;
        case 2:
            ghost.directionSprite = 4;
            ghost.velocity_y -= 2;
            break;
        case 3:
            ghost.directionSprite = 6;
            ghost.velocity_y += 2;
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
    SDL_Point texture_center = {ghost.texture.x + Pacman::width / 2,
                                ghost.texture.y + Pacman::height / 2};

    // TODO: remove code dupplication here and in pacman class
    int i = (texture_center.x - b.getPos().x) / Box::size;
    int j = (texture_center.y - b.getPos().y) / Box::size;
    if (ghost.velocity_x) {
        ghost.texture.x += ghost.velocity_x;
        // fix y position
        SDL_Point curr_box_center = {
            b.getPos().x + i * Box::size + Box::size / 2,
            b.getPos().y + j * Box::size + Box::size / 2};
        int correct_y = curr_box_center.y - ghost.texture.h / 2;
        ghost.texture.y = correct_y;

        if ((ghost.texture.x < border.x) ||
            (ghost.texture.x + ghost.texture.w > border.x + border.w) ||
            checkCollision(ghost.texture, b, Box::Type::wall)) {
            ghost.texture.x -= ghost.velocity_x;
        }
    } else if (ghost.velocity_y) {
        ghost.texture.y += ghost.velocity_y;

        // fix x position
        SDL_Point curr_box_center = {
            b.getPos().x + i * Box::size + Box::size / 2,
            b.getPos().y + j * Box::size + Box::size / 2};
        int correct_x = curr_box_center.x - ghost.texture.w / 2;
        ghost.texture.x = correct_x;

        if ((ghost.texture.y < border.y) ||
            (ghost.texture.y + ghost.texture.h) > border.y + border.h ||
            checkCollision(ghost.texture, b, Box::Type::wall)) {
            ghost.texture.y -= ghost.velocity_y;
        }
    }

    // Make collision only point so textures can overlap
    SDL_Point collison{ghost.texture.x + ghost.texture.w / 2,
                       ghost.texture.y + ghost.texture.h / 2};
    if (SDL_PointInRect(&collison, &pacman.getCollision())) {
        pacman.clearState(true);
    }
}

/*
    struct BoxPos {
        int x;
        int y;
    };

    // TODO: there should be function to take box position of entity
    BoxPos pacmanPos = {
        (pacman.getPos().x - b.getPos().x) / Box::size,
        (pacman.getPos().y - b.getPos().y) / Box::size,
    };
    BoxPos ghostPos = {
        (ghost.getPos().x - b.getPos().x) / Box::size,
        (ghost.getPos().y - b.getPos().y) / Box::size,
    };

    std::vector<Graph::BoxNode *> path = breadthFirstSearch(
        b.graph, ghostPos.x, ghostPos.y, pacmanPos.x, pacmanPos.y);
    printf("\n");
    for (const Graph::BoxNode *box : path) {
        printf(" -> (%d, %d)", box->x, box->y);
    }
    printf("\n");

*/

void GhostDeathBehavior::loadPathToHome(Board &b) {

    struct BoxPos {
        int x, y;
    };

    BoxPos ghostPos = {
        (ghost.getPos().x - b.getPos().x) / Box::size,
        (ghost.getPos().y - b.getPos().y) / Box::size,
    };
    BoxPos basePos = {
        (b.getGhostStart().x - b.getPos().x) / Box::size,
        (b.getGhostStart().y - b.getPos().y) / Box::size,
    };

    path = breadthFirstSearch(b.graph, ghostPos.x, ghostPos.y, basePos.x,
                              basePos.y);
}

// TODO: return to the base as dot
void GhostDeathBehavior::move(Board &b, Entity &e) {
    // TODO: Traverse home using path
    printf("\nghost die\n");
    for (auto &p : path) {
        printf(" -> (%d, %d)", p->x, p->y);
    }
    printf("\n");
    ghost.setPos(b.getGhostStart());
    // TODO:
    // when return to base switch behavior
}
