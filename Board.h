#ifndef PACMAN_BOARD_H
#define PACMAN_BOARD_H

#include "Box.h"
#include "Game.h"
#include "LTexture.h"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

// TODO: change name to Level
class Board {
  public:
    static LTexture mapTexture;
    static constexpr int clipsNumber = 44;
    static SDL_Rect mapClips[clipsNumber];

    Board(int h, int w)
        : rows_(h), columns_(w),
          border{Game::screen_width / 2 - (rows() * Box::size) / 2,
                 Game::screen_height / 2 - (columns() * Box::size) / 2,
                 columns() * Box::size, rows() * Box::size},
          board_(w * h) {}

    Board(const std::string &filename);

    SDL_Point getPos() const {
        SDL_Point p{border.x, border.y};
        return p;
    }

    void render();

    int rows() const { return rows_; }
    int columns() const { return columns_; }
    Box &getBox(int i, int j) { return board_.at(i * columns() + j); }

    int getTotalPoints() const { return totalPoints; }

  private:
    int rows_;
    int columns_;
    int totalPoints = 0;

    SDL_Rect border;

    std::vector<Box> board_;
};

#endif // !PACMAN_BOARD_H
