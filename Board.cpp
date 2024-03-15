#include "Board.h"
#include "Game.h"
#include <cstdio>
#include <fstream>
#include <queue>
#include <stdexcept>
#include <unordered_map>

using namespace std;

SDL_Rect Board::mapClips[Board::clipsNumber];
LTexture Board::mapTexture;

void Board::render() {
    SDL_Point box{border.x, border.y};
    for (int y = 0; y < rows(); ++y) {
        for (int x = 0; x < columns(); ++x) {
            getBox(x, y).render(box.x, box.y);
            box.x += Box::size;
        }
        box.x = border.x;
        box.y += Box::size;
    }

    SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRect(Game::gRenderer, &border);
}

Board::Board(const std::string &filename) {
    ifstream is{filename};
    if (!is) {
        throw std::runtime_error{"can't open: " + filename};
    }

    is >> this->rows_ >> this->columns_;
    board_.resize(columns_ * rows_);

    border.x = Game::screen_width / 2 - (columns() * Box::size) / 2;
    border.y = Game::screen_height / 2 - (rows() * Box::size) / 2;
    border.h = rows() * Box::size;
    border.w = columns() * Box::size;

    // double because it is relative to box position instead of pixels
    double pacmanStart_x, pacmanStart_y;
    is >> pacmanStart_x >> pacmanStart_y;
    pacman_start.x = border.x + Box::size * (pacmanStart_x - 1);
    pacman_start.y = border.y + Box::size * (pacmanStart_y - 1);

    double ghostStart_x, ghostStart_y;
    is >> ghostStart_x >> ghostStart_y;
    ghost_start.x = border.x + Box::size * (ghostStart_x - 1);
    ghost_start.y = border.y + Box::size * (ghostStart_y - 1);

    char type;
    for (int y = 0; y < rows(); ++y) {
        for (int x = 0; x < columns(); ++x) {
            is >> type;
            switch (type) {
            case '#':
                getBox(x, y).setType(Box::Type::wall);
                // NOTE: for now disable map
                // TODO: algorithm that picks appriopriate clip
                // depending on neightbours
                // getBox(i, j).setClip(&Board::mapClips[0]);
                break;
            case '.':
                ++totalPoints;
                getBox(x, y).setType(Box::Type::point);
                break;
            case '*':
                ++totalPoints;
                getBox(x, y).setType(Box::Type::super_point);
                break;
            case '-':
                getBox(x, y).setType(Box::Type::empty);
                break;
            }
        }
    }

    graph.setBoard(*this);
}
