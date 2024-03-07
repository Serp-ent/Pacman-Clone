#ifndef PACMAN_BOARD_H
#define PACMAN_BOARD_H

#include <SDL2/SDL.h>
#include <vector>
#include "Box.h"

// HACK: maybe use some common header
extern const int screen_width;
extern const int screen_height;

class Board {
  public:
    Board(int w, int h)
        : rows_(h), columns_(h),
          border{screen_width / 2 - (rows() * Box::size) / 2,
                 screen_height / 2 - (columns() * Box::size) / 2,
                 columns() * Box::size, rows() * Box::size},
          board_(w * h) {}

    SDL_Point getPos() const {
        SDL_Point p{border.x, border.y};
        return p;
    }

    void render();

    int rows() const { return rows_; }
    int columns() const { return columns_; }
    Box &getBox(int i, int j) { return board_.at(i * rows() + j); }

  private:
    const int rows_;
    const int columns_;

    const SDL_Rect border;

    std::vector<Box> board_;
};

#endif // !PACMAN_BOARD_H
