#ifndef PACMAN_BOARD_H
#define PACMAN_BOARD_H

#include "Box.h"
#include "Game.h"
#include "Graph.h"
#include "LTexture.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
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
          board_(w * h) {
        graph.setBoard(*this);
    }

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

    const SDL_Point &getPacmanStart() const { return pacman_start; }
    const SDL_Point &getGhostStart() const { return ghost_start; }

    // TODO: move to private section
    Graph graph;
  private:
    int rows_;
    int columns_;
    int totalPoints = 0;

    SDL_Point pacman_start;
    SDL_Point ghost_start;


    SDL_Rect border;

    std::vector<Box> board_;
};

#endif // !PACMAN_BOARD_H
