#include "Behaviors.h"
#include <algorithm>
#include <random>
#include <unistd.h>

#include "Board.h"
#include "Graph.h"
#include "SDL2/SDL.h"
#include "utils.h"
#include <SDL2/SDL_rect.h>
#include <cstdio>
#include <vector>

Graph::BoxNode *pickRandomCorridor(std::vector<Graph::BoxNode *> &path,
                                   Graph::BoxNode *dest) {
    std::vector<Graph::BoxNode *> filtered;
    for (Graph::BoxNode *v : path.back()->neighbors) {
        if (v == dest || v == path.at(path.size() - 2)) {
            continue;
        }

        filtered.push_back(v);
    }

    if (!filtered.empty()) {
        return filtered.at(getRandomNumber(0, filtered.size()));
    } else {
        return nullptr;
    }
}

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

    if (pacman.attackerTime.getTicks() > 4'000) {
        Ghost &g = dynamic_cast<Ghost &>(ghost);
        if (pacman.attackerTime.getTicks() < 5'000) {
            g.setBlinking();
            // start blinking
        } else {
            pacman.attackerTime.stop();
            g.setBlinking(false);
            if (!ghost.wasKilled()) { // check if ghost was returning to base
                ghost.setAttack();
            }
            pacman.setAttack(false);
        }
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
    if (!ghost.wasKilled() &&
        SDL_PointInRect(&collison, &pacman.getCollision())) {
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
        //
        ghost.velocity_y = 0;
        ghost.velocity_x = 0;
        switch (direction % 4) {
        case 0:
            ghost.directionSprite = Entity::right;
            ghost.velocity_x += Entity::velocity;
            break;
        case 1:
            ghost.directionSprite = Entity::left;
            ghost.velocity_x -= Entity::velocity;
            break;
        case 2:
            ghost.directionSprite = Entity::up;
            ghost.velocity_y -= Entity::velocity;
            break;
        case 3:
            ghost.directionSprite = Entity::down;
            ghost.velocity_y += Entity::velocity;
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
    struct BoxPos {
        int x, y;
    };
    BoxPos ghostPos = {
        (ghost.getPos().x - b.getPos().x + Ghost::width / 2) / Box::size,
        (ghost.getPos().y - b.getPos().y + Ghost::height / 2) / Box::size,
    };

    constexpr int deadGhostSpeed = 4;

    Graph::BoxNode *dest = path.back();
    ghost.velocity_y = 0;
    ghost.velocity_x = 0;
    if (dest->x > ghostPos.x) {
        ghost.velocity_x += deadGhostSpeed;
    } else if (dest->x < ghostPos.x) {
        ghost.velocity_x -= deadGhostSpeed;
    } else if (dest->y < ghostPos.y) {
        ghost.velocity_y -= deadGhostSpeed;
    } else if (dest->y > ghostPos.y) {
        ghost.velocity_y += deadGhostSpeed;
    }

    if (dest->x == ghostPos.x && dest->y == ghostPos.y) {
        path.pop_back();
        if (path.empty()) {
            printf("[GHOST]: already at destination\n");
            printf("[GHOST]: Reseting to aggressive a behavior\n");
            ghost.clearState();
            ghost.setAttack();
            return;
        }
    }

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
}

void CyanGhostBehavior::move(Board &b, Entity &pacman) {
    static int i = 0;
    ++i;
    if (i > 100) {
        printf("Cyan::Behavior ghost not implemented\n");
        i = 0;
    }
}

void PinkGhostBehavior::move(Board &b, Entity &pacman) {
    struct BoxPos {
        int x, y;
    };
    BoxPos ghostPos = {
        (ghost.getPos().x - b.getPos().x + Ghost::width / 2) / Box::size,
        (ghost.getPos().y - b.getPos().y + Ghost::height / 2) / Box::size,
    };

    if (path.empty()) {
        BoxPos basePos = {
            (pacman.getPos().x - b.getPos().x) / Box::size,
            (pacman.getPos().y - b.getPos().y) / Box::size,
        };

        constexpr int tileDistance = 4;
        BoxPos targetPos = basePos;
        switch (pacman.getDirection()) {
        case Entity::Direction::up:
            targetPos.y -= tileDistance;
            break;
        case Entity::Direction::down:
            targetPos.y += tileDistance;
            break;
        case Entity::Direction::left:
            targetPos.x -= tileDistance;
            break;
        case Entity::Direction::right:
            targetPos.x += tileDistance;
            break;
        }

        bool targetReachable =
            (b.getBox(targetPos.x, targetPos.y).getType() != Box::Type::wall);
        if (targetReachable) {
            path = breadthFirstSearch(b.graph, ghostPos.x, ghostPos.y,
                                      targetPos.x, targetPos.y);
        } else {
            path = breadthFirstSearch(b.graph, ghostPos.x, ghostPos.y,
                                      basePos.x, basePos.y);
        }
    }

    Graph::BoxNode *dest = path.back();
    ghost.velocity_y = 0;
    ghost.velocity_x = 0;
    if (dest->x > ghostPos.x) {
        ghost.directionSprite = Entity::right;
        ghost.velocity_x += Entity::velocity;
    } else if (dest->x < ghostPos.x) {
        ghost.directionSprite = Entity::left;
        ghost.velocity_x -= Entity::velocity;
    } else if (dest->y < ghostPos.y) {
        ghost.directionSprite = Entity::up;
        ghost.velocity_y -= Entity::velocity;
    } else if (dest->y > ghostPos.y) {
        ghost.directionSprite = Entity::down;
        ghost.velocity_y += Entity::velocity;
    }

    if (dest->x == ghostPos.x && dest->y == ghostPos.y) {
        path.pop_back();
    }

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

void OrangeGhostBehavior::move(Board &b, Entity &pacman) {
    struct BoxPos {
        int x, y;
    };
    BoxPos ghostPos = {
        (ghost.getPos().x - b.getPos().x + Ghost::width / 2) / Box::size,
        (ghost.getPos().y - b.getPos().y + Ghost::height / 2) / Box::size,
    };
    BoxPos basePos = {
        (pacman.getPos().x - b.getPos().x) / Box::size,
        (pacman.getPos().y - b.getPos().y) / Box::size,
    };

    constexpr int chaseThreshold = 8;
    bool chasePacman = manhattanDistance(ghostPos.x, ghostPos.y, basePos.x,
                                         basePos.y) > chaseThreshold;
    if (scared && !path.empty()) {
        // pass - do nothing keep following path
    } else if (chasePacman) {
        if (scared) {
            // state changes find new shortest path
            path = breadthFirstSearch(b.graph, ghostPos.x, ghostPos.y,
                                      basePos.x, basePos.y);
            scared = false;
        }
        // follow shortest path
        // chase pacman
    } else {
        if (!scared) {
            // find shortest path
            // TODO: map should be responsible for providing safe points for
            // orange ghost, this should be read to static ghost vector
            std::vector<BoxPos> safezones = {
                {1, 1},                          // top left
                {b.columns() - 2, 1},            // top right
                {1, b.rows() - 2},               // bottom left
                {b.columns() - 2, b.rows() - 2}, // bottom right
            };

            const BoxPos *pos =
                &safezones[getRandomNumber(0, safezones.size())];
            b.getBox(pos->x, pos->y).setType(Box::Type::empty);
            printf("Run away to (%d, %d)\n", pos->x, pos->y);
            path = breadthFirstSearch(b.graph, ghostPos.x, ghostPos.y, pos->x,
                                      pos->y);
            scared = true;
        }
        // get one random position from 4 bases to where orange ghosts return to
    }

    if (path.empty()) {
        // guard to prevent segfault for empty path
        path = breadthFirstSearch(b.graph, ghostPos.x, ghostPos.y, basePos.x,
                                  basePos.y);
        scared = false;
    }

    ghost.velocity_y = 0;
    ghost.velocity_x = 0;
    Graph::BoxNode *dest = path.back();
    if (dest->x > ghostPos.x) {
        ghost.directionSprite = Entity::right;
        ghost.velocity_x += Entity::velocity;
    } else if (dest->x < ghostPos.x) {
        ghost.directionSprite = Entity::left;
        ghost.velocity_x -= Entity::velocity;
    } else if (dest->y < ghostPos.y) {
        ghost.directionSprite = Entity::up;
        ghost.velocity_y -= Entity::velocity;
    } else if (dest->y > ghostPos.y) {
        ghost.directionSprite = Entity::down;
        ghost.velocity_y += Entity::velocity;
    }

    if (dest->x == ghostPos.x && dest->y == ghostPos.y) {
        path.pop_back();
    }

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

void RedGhostBehavior::move(Board &b, Entity &pacman) {
    struct BoxPos {
        int x, y;
    };
    BoxPos ghostPos = {
        (ghost.getPos().x - b.getPos().x + Ghost::width / 2) / Box::size,
        (ghost.getPos().y - b.getPos().y + Ghost::height / 2) / Box::size,
    };

    if (path.empty()) {
        BoxPos basePos = {
            (pacman.getPos().x - b.getPos().x) / Box::size,
            (pacman.getPos().y - b.getPos().y) / Box::size,
        };

        path = breadthFirstSearch(b.graph, ghostPos.x, ghostPos.y, basePos.x,
                                  basePos.y);
    }

    Graph::BoxNode *dest = path.back();
    ghost.velocity_y = 0;
    ghost.velocity_x = 0;
    if (dest->x > ghostPos.x) {
        ghost.directionSprite = Entity::right;
        ghost.velocity_x += Entity::velocity;
    } else if (dest->x < ghostPos.x) {
        ghost.directionSprite = Entity::left;
        ghost.velocity_x -= Entity::velocity;
    } else if (dest->y < ghostPos.y) {
        ghost.directionSprite = Entity::up;
        ghost.velocity_y -= Entity::velocity;
    } else if (dest->y > ghostPos.y) {
        ghost.directionSprite = Entity::down;
        ghost.velocity_y += Entity::velocity;
    }

    if (dest->x == ghostPos.x && dest->y == ghostPos.y) {
        path.pop_back();
    }

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

void GhostRunAwayBehavior::move(Board &b, Entity &pacman) {
    struct BoxPos {
        int x, y;
    };

    BoxPos ghostPos = {
        (ghost.getPos().x - b.getPos().x + Ghost::width / 2) / Box::size,
        (ghost.getPos().y - b.getPos().y + Ghost::height / 2) / Box::size,
    };
    BoxPos pacmanPos = {
        (pacman.getPos().x - b.getPos().x) / Box::size,
        (pacman.getPos().y - b.getPos().y) / Box::size,
    };

    // TODO: reset path sometimes
    if (path.empty()) {
        path = breadthFirstSearch(b.graph, ghostPos.x, ghostPos.y, pacmanPos.x,
                                  pacmanPos.y);
    }

    Graph::BoxNode *dest = path.back();
    if (dest->x == ghostPos.x && dest->y == ghostPos.y) {

        // TODO: pop front from path or check if we have shortert path while
        // running away to prevent going straight to pacman
        dest = pickRandomCorridor(path, dest);
        if (dest == nullptr) {
            dest = path.at(path.size() - 2);
        }
        path.push_back(dest);
    }

    ghost.velocity_y = 0;
    ghost.velocity_x = 0;
    if (dest->x > ghostPos.x) {
        ghost.directionSprite = Entity::right;
        ghost.velocity_x += Entity::velocity;
    } else if (dest->x < ghostPos.x) {
        ghost.directionSprite = Entity::left;
        ghost.velocity_x -= Entity::velocity;
    } else if (dest->y < ghostPos.y) {
        ghost.directionSprite = Entity::up;
        ghost.velocity_y -= Entity::velocity;
    } else if (dest->y > ghostPos.y) {
        ghost.directionSprite = Entity::down;
        ghost.velocity_y += Entity::velocity;
    }

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
}
