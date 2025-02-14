#ifndef PACMAN_UTILS_H
#define PACMAN_UTILS_H

#include "Box.h"

bool checkCollision(const SDL_Rect &a, const SDL_Rect &b);

Box *checkCollision(SDL_Rect &p, Board &b, Box::Type boxType);

Box *pointIsReached(SDL_Rect &p, Board &b, Box::Type boxType);

int manhattanDistance(int x1, int y1, int x2, int y2);

int getRandomNumber(int min, int max);

#endif // !PACMAN_UTILS_H
