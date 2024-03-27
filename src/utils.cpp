#include "utils.h"

#include "Board.h"
#include <random>
#include "Box.h"

int manhattanDistance(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

bool checkCollision(const SDL_Rect &a, const SDL_Rect &b) {
    // The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    // Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    // Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;
    // If any of the sides from A are outside of B
    if (bottomA <= topB) {
        return false;
    }

    if (topA >= bottomB) {
        return false;
    }

    if (rightA <= leftB) {
        return false;
    }

    if (leftA >= rightB) {
        return false;
    }

    // If none of the sides from A are outside B
    return true;
}

Box *checkCollision(SDL_Rect &p, Board &b, Box::Type boxType) {
    // TODO: check only surrounding boxes
    SDL_Rect box{b.getPos().x, b.getPos().y, Box::size, Box::size};
    for (int y = 0; y < b.rows(); ++y) {
        for (int x = 0; x < b.columns(); ++x) {
            if (b.getBox(x, y).getType() == boxType && checkCollision(p, box)) {
                return &b.getBox(x, y);
            }

            box.x += Box::size;
        }

        box.x = b.getPos().x;
        box.y += Box::size;
    }

    return nullptr;
    // collision boxes
}

Box *pointIsReached(SDL_Rect &p, Board &b, Box::Type boxType) {
    // TODO: check only surrounding boxes

    SDL_Rect box{b.getPos().x, b.getPos().y, Box::size, Box::size};
    for (int y = 0; y < b.rows(); ++y) {
        for (int x = 0; x < b.columns(); ++x) {
            SDL_Point box_center{box.x + Box::size / 2, box.y + Box::size / 2};
            if (b.getBox(x, y).getType() == boxType &&
                SDL_PointInRect(&box_center, &p)) {

                return &b.getBox(x, y);
            }

            box.x += Box::size;
        }

        box.x = b.getPos().x;
        box.y += Box::size;
    }

    return nullptr;
}

int getRandomNumber(int min, int max) {
    // Create a random device to seed the random number generator
    std::random_device rd;

    // Create a random number generator
    std::mt19937 gen(rd());

    // Create a distribution to define the range of random numbers
    std::uniform_int_distribution<int> dis(min, max - 1);

    // Generate and return a random number
    return dis(gen);
}
