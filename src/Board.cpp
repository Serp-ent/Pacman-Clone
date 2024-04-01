#include "Board.h"
#include "Box.h"
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
            case 'o':
                getBox(x, y).setType(Box::Type::ghostExit);
                break;
            }
        }
    }

    for (int y = 0; y < rows(); ++y) {
        for (int x = 0; x < columns(); ++x) {
            if (getBox(x, y).getType() == Box::Type::wall) {
                Box::WallType walltype =
                    getWallClipBasedOnNeighbours(*this, x, y);

                if (walltype == Box::Junk5) {
                    getBox(x, y).setClip(nullptr);
                } else {
                    getBox(x, y).setClip(&mapClips[walltype]);
                }
            } else if (getBox(x, y).getType() == Box::Type::ghostExit) {
                // by default assume its left
                Box::WallType ghostExitSide = Box::WallType::GhostExitLeft;
                if (getBox(x - 1, y).getType() == Box::Type::ghostExit) {
                    ghostExitSide = Box::WallType::GhostExitRight;
                }

                getBox(x, y).setClip(&mapClips[ghostExitSide]);
            }
        }
    }

    graph.setBoard(*this);
}

Box::WallType getWallClipBasedOnNeighbours(Board &b, int x, int y) {
    using byte = unsigned char;

    bool isBorder = false;
    byte bitmask = 0;
    int bit = 7; // most significant bit

    isBorder = (x == 0 || x == b.columns() - 1 || y == 0 || y == b.rows() - 1);
    for (int i = y - 1; i < y + 2; ++i) {
        for (int j = x - 1; j < x + 2; ++j) {
            bool isItSelf = (j == x && i == y);
            if (isItSelf) {
                continue;
            }

            if ((j < 0 || j > b.columns() - 1) || (i < 0 || i > b.rows() - 1)) {
                --bit;
                continue;
            }
            if (b.getBox(j, i).getType() == Box::Type::wall ||
                b.getBox(j, i).getType() == Box::Type::ghostExit) {
                bitmask |= (1 << bit);
            }

            --bit;
        }
    }

    // TODO: refactor
    // TODO: fix magic junk5 variable
    if (isBorder) {
        switch (bitmask) {
        case 0b00001010:
            return Box::WallType::MapTopLeftCorner;
        case 0b00010010:
            return Box::WallType::MapTopRightCorner;
        case 0b01001000:
            return Box::WallType::MapBottomLeftCorner;
        case 0b01010000:
            return Box::WallType::MapBottomRightCorner;
        //****************************************************
        case 0b00011000:
            return b.getBox(x - 1, y).getClip();
        case 0b00011100:
        case 0b00011001:
            return Box::WallType::MapTopWall;
        case 0b00111000:
        case 0b10011000:
            return Box::WallType::MapBottomWall;

        case 0b01000010: { // specify if its left or right wall based on upper
                           // wall
            return b.getBox(x, y - 1).getClip();
        }
        case 0b01100010:
        case 0b01000011:
            return Box::WallType::MapLeftWall;
        case 0b01000110:
        case 0b11000010:
            return Box::WallType::MapRightWall;
        case 0b01001011:
            return Box::WallType::MapConcaveBottomLeft;
        case 0b01010110:
            return Box::WallType::MapConcaveBottomRight;
        case 0b11010010:
            return Box::WallType::MapConcaveTopRight;
        case 0b01101010:
            return Box::WallType::MapConcaveTopLeft;
        case 0b01101011:
            return Box::WallType::MapLeftWallSingle;
        case 0b11010110:
            return Box::WallType::MapRightWallSingle;
        case 0b00011011:
            return Box::WallType::MapConcaveTopRightUpper;
        case 0b00011110:
            return Box::WallType::MapConcaveTopLeftUpper;
        case 0b01101000:
            return Box::WallType::MapBorderOutCorridorLeftTop;
        case 0b00010110:
            return Box::WallType::MapBorderOutCorridorRightBottom;
        case 0b00001011:
            return Box::WallType::MapBorderOutCorridorLeftBottom;
        case 0b11010000:
            return Box::WallType::MapBorderOutCorridorRightTop;
        }
    } else {
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
        case 0b00001010:
            return Box::WallType::TopLeftCorner;
        case 0b00010010:
            return Box::WallType::TopRightCorner;
        case 0b01001000:
            return Box::WallType::BottomLeftCorner;
        case 0b01010000:
            return Box::WallType::BottomRightCorner;
        //****************************************************
        case 0b00011000:
            if (b.getBox(x - 1, y).getType() == Box::Type::ghostExit) {
                return b.getBox(x - 3, y).getClip();
            }
            return b.getBox(x - 1, y).getClip();
        case 0b00011100:
        case 0b00011001:
            return Box::WallType::TopWall;
        case 0b00111000:
        case 0b10011000:
            return Box::WallType::BottomWall;

        case 0b01000010: { // specify if its left or right wall based on upper
                           // wall
            return b.getBox(x, y - 1).getClip();
        }
        case 0b01100010:
        case 0b01000011:
            return Box::WallType::LeftWall;
        case 0b01000110:
        case 0b11000010:
            return Box::WallType::RightWall;
        // **************************************************************
        case 0b11111111:
            return Box::WallType::InsideWall;
        }
    }
    return Box::WallType::Junk5;
}
