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

    for (int y = 0; y < rows(); ++y) {
        for (int x = 0; x < columns(); ++x) {
            if (getBox(x, y).getType() == Box::Type::wall) {
                Box::WallType walltype =
                    getWallClipBasedOnNeighbours(*this, x, y);
                if (walltype == Box::Junk1) {
                    getBox(x, y).setClip(nullptr);
                } else {
                    getBox(x, y).setClip(&mapClips[walltype]);
                }
            }
        }
    }

    graph.setBoard(*this);
}

Box::WallType getWallClipBasedOnNeighbours(Board &b, int x, int y) {
    if (x == 0) {
        if (y == 0) {
            return Box::WallType::MapTopLeftCorner;
        } else if (y == b.rows() - 1) {
            return Box::WallType::MapBottomLeftCorner;
        }

        return Box::WallType::MapLeftWall;
    }
    if (x == (b.columns() - 1)) {
        if (y == 0) {
            return Box::WallType::MapTopRightCorner;
        } else if (y == b.rows() - 1) {
            return Box::WallType::MapBottomRightCorner;
        }

        return Box::WallType::MapRightWall;
    }

    if (y == 0) {
        return Box::WallType::MapTopWall;
    }
    if (y == (b.rows() - 1)) {
        return Box::WallType::MapBottomWall;
    }

    //*************************************************************

    // TODO: different algorithm (try traverse along border)

    using byte = unsigned char;
    byte bitmask = 0;
    int bit = 7; // most significant bit
    for (int i = y - 1; i < y + 2; ++i) {
        for (int j = x - 1; j < x + 2; ++j) {
            bool isItSelf = (j == x && i == y);
            if (isItSelf) {
                continue;
            }

            if (b.getBox(j, i).getType() == Box::Type::wall) {
                bitmask |= (1 << bit);
            }

            --bit;
        }
    }

    switch (bitmask) {
    case 0b00001011:
        return Box::WallType::TopLeftCorner;
    case 0b00010110:
        return Box::WallType::TopRightCorner;
    case 0b01101000:
        return Box::WallType::BottomLeftCorner;
    case 0b11010000:
        return Box::WallType::BottomRightCorner;
    case 0b11111000:
    case 0b11111001:
    case 0b11111100:
        return Box::WallType::BottomWall;
    case 0b00011111:
        // close to concave box
    case 0b00111111:
    case 0b10011111:
        return Box::WallType::TopWall;
    case 0b01101011:
    case 0b11101011:
    case 0b01101111:
        return Box::WallType::LeftWall;
    case 0b11010110:
    case 0b11110110:
    case 0b11010111:
        return Box::WallType::RightWall;
    case 0b11111110:
        return Box::WallType::ConcaveTopLeft;
    case 0b11111011:
        return Box::WallType::ConcaveTopRight;
    case 0b01111111:
        return Box::WallType::ConcaveBottomLeft;
    case 0b11011111:
        return Box::WallType::ConcaveBottomRight;
    // **** EXCEPTION FOR GHOST BOX *************************************
    case 0b01000010:
    case 0b11000010:
    case 0b01100010:
    case 0b01000110:
    case 0b01000011:
        return Box::WallType::RightWall;
    case 0b00001010:
        return Box::WallType::TopLeftCorner;
    case 0b00010010:
        return Box::WallType::TopRightCorner;
    case 0b01001000:
        return Box::WallType::BottomLeftCorner;
    case 0b01010000:
        return Box::WallType::BottomRightCorner;
    case 0b00011000:
    case 0b10011000:
    case 0b00111000:
    case 0b00011100:
    case 0b00011001:
        return Box::WallType::TopWall;
    // **************************************************************
    case 0b11111111:
        return Box::WallType::InsideWall;
    }

    return Box::WallType::Junk1;
}
