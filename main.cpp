#include "Board.h"
#include "Box.h"
#include "Ghost.h"
#include "Hud.h"
#include "Pacman.h"
#include "TextTexture.h"
#include "Timer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Game.h"

// TODO: maybe use a namespace to limit namespace pollution

// TODO: create MENU and HUD

// TODO: create level designer in SDL2 that creates textfile with map
// TODO: use SDL functions with unique_ptr

int totalPoints = 0;

// TODO: inherit from Enitty object
// create GHOST enemy

// TODO: don't return by copy
// TODO: check ways to code board in file e.g ASCII or numbers
Board createLevel1() {
    // BUG: exception thrown when rows < columns
    Board level(28, 28);
    for (int i = 0; i < level.rows(); ++i) {
        for (int j = 0; j < level.columns(); ++j) {
            ++totalPoints;
            level.getBox(i, j).setType(Box::Type::point);
        }
    }

    // draw borders
    for (int i = 0; i < level.rows(); ++i) {
        level.getBox(i, 0).setType(Box::Type::wall);
        level.getBox(i, level.columns() - 1).setType(Box::Type::wall);
        --totalPoints;
        --totalPoints;
    }
    for (int i = 0; i < level.columns(); ++i) {
        level.getBox(0, i).setType(Box::Type::wall);
        level.getBox(level.rows() - 1, i).setType(Box::Type::wall);
        --totalPoints;
        --totalPoints;
    }

    //*********************************************************************************

    for (int i = 2; i < 6; ++i) {
        for (int j = 2; j < 5; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }
    for (int i = 7; i < 12; ++i) {
        for (int j = 2; j < 5; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 13; i < 15; ++i) {
        for (int j = 1; j < 5; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 16; i < 21; ++i) {
        for (int j = 2; j < 5; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }
    for (int i = 22; i < 26; ++i) {
        for (int j = 2; j < 5; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    //*********************************************************************************

    for (int i = 2; i < 6; ++i) {
        for (int j = 6; j < 8; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }
    for (int i = 7; i < 9; ++i) {
        for (int j = 6; j < 8; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 10; i < 18; ++i) {
        for (int j = 6; j < 8; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 19; i < 21; ++i) {
        for (int j = 6; j < 8; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 22; i < 26; ++i) {
        for (int j = 6; j < 8; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    //*********************************************************************************

    for (int i = 1; i < 6; ++i) {
        for (int j = 9; j < 14; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 9; i < 12; ++i) {
        for (int j = 9; j < 11; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }
    for (int i = 16; i < 19; ++i) {
        for (int j = 9; j < 11; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 7; i < 9; ++i) {
        for (int j = 9; j < 14; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 7; i < 9; ++i) {
        level.getBox(8, i).setType(Box::Type::wall);
        --totalPoints;
    }
    for (int i = 19; i < 21; ++i) {
        level.getBox(8, i).setType(Box::Type::wall);
        --totalPoints;
    }
    for (int i = 13; i < 15; ++i) {
        level.getBox(8, i).setType(Box::Type::wall);
        --totalPoints;
    }
    for (int i = 13; i < 15; ++i) {
        for (int j = 9; j < 11; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 19; i < 21; ++i) {
        for (int j = 9; j < 14; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    for (int i = 22; i < 27; ++i) {
        for (int j = 9; j < 14; ++j) {
            level.getBox(j, i).setType(Box::Type::wall);
            --totalPoints;
        }
    }

    //*********************************************************************************
    // Draw box;
    for (int i = 10; i < 18; ++i) {
        if (i == 13 || i == 14) {
            level.getBox(12, i).setType(Box::Type::empty);
            continue;
        }
        level.getBox(12, i).setType(Box::Type::wall);
    }
    for (int i = 12; i < 16; ++i) {
        level.getBox(i, 10).setType(Box::Type::wall);
    }
    for (int i = 10; i < 18; ++i) {
        level.getBox(16, i).setType(Box::Type::wall);
    }
    for (int i = 12; i < 16; ++i) {
        level.getBox(i, 17).setType(Box::Type::wall);
    }

    for (int i = 11; i < 17; ++i) {
        for (int j = 13; j < 16; ++j) {
            level.getBox(j, i).setType(Box::Type::empty);
        }
    }
    //*********************************************************************************

    return level;
}

int main() {
    Game game;
    bool quit = false;
    SDL_Event e;
    Board board = createLevel1();

    SDL_Point start_pos = board.getPos();
    // TODO: change pacman start position to be inside box
    start_pos.x += (board.columns() / 2 * Box::size);
    start_pos.y += (board.rows() * Box::size - 8 * Box::size);
    Pacman pacman(game.get_points_ref(), start_pos.x, start_pos.y);

    start_pos = board.getPos();
    start_pos.x += (board.columns() / 2 * Box::size);
    start_pos.y += (13 * Box::size);
    Ghost ghost(start_pos.x, start_pos.y);

    // TODO: create HUD class
    SDL_Color black{0xFF, 0xFF, 0xFF, 0xFF};

    TextTexture theEndText;
    theEndText.loadText("The END", black);

    Hud hud;
    Timer fpsTimer;
    int frames = 0;
    fpsTimer.start();
    while (!quit) {
        // TODO: DON't start game until user presses key
        // pacman should be default sprite (yellow ball) until game has
        // started
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (!pacman.playsDeathAnimation()) {
                pacman.handleEvent(e);
            }
        }
        if (game.isEnd()) {
            SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
            SDL_RenderClear(Game::gRenderer);

            // theEndText.render(
            //     Game::screen_width / 2 - theEndText.getWidth() / 2,
            //     Game::screen_height / 2 - theEndText.getHeight() / 2 -
            //         hud.points_texture.getHeight());
            // hud.points_texture.render(
            //     Game::screen_width / 2 - hud.points_texture.getWidth() /
            //     2, Game::screen_height / 2 -
            //     hud.points_texture.getHeight() / 2);
            SDL_RenderPresent(Game::gRenderer);
            continue;
        }

        float avgFPS = frames / (fpsTimer.getTicks() / 1000.f);
        if (avgFPS > 2'000'000) {
            avgFPS = 0;
        }

        hud.setPoints(game.get_points());
        hud.setFps(static_cast<int>(avgFPS));
        hud.setLivesLeft(pacman.getLifesLeft());

        // clear screen
        SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(Game::gRenderer);

        hud.render();

        board.render();

        pacman.render();
        ghost.render();

        // update screen
        SDL_RenderPresent(Game::gRenderer);
        ++frames;

        if (pacman.wasKilled()) {
            if (pacman.playsDeathAnimation()) {
                continue;
            }

            pacman.setLifesLeft(pacman.getLifesLeft() - 1);

            if (pacman.getLifesLeft() == 0) {
                game.setEnd();
                continue;
            }
            // TODO: set position (the map/level should know where pacman
            // and ghost start position is)
            int lives = pacman.getLifesLeft();
            // HACK: reset pacman and ghost position
            start_pos = board.getPos();
            start_pos.x += (board.columns() / 2 * Box::size);
            start_pos.y += (board.rows() * Box::size - 8 * Box::size);
            pacman = Pacman(game.get_points_ref(), start_pos.x, start_pos.y);
            pacman.setLifesLeft(lives);

            start_pos.x += (board.columns() / 2 * Box::size);
            start_pos.y += (13 * Box::size);
            ghost = Ghost(start_pos.x, start_pos.y);

        } else if (game.get_points() == totalPoints) {
            game.setEnd();
        } else {
            pacman.move(board);
            ghost.move(board, pacman);
        }
    }
}
