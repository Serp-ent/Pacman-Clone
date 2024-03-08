#include "Board.h"
#include "Box.h"
#include "Ghost.h"
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

// TODO: create MENU and HUD

// TODO: create level designer in SDL2 that creates textfile with map
// TODO: use SDL functions with unique_ptr

int totalPoints = 0;

// TODO: inherit from Enitty object
// create GHOST enemy

// TODO: don't return by copy
// TODO: check ways to code board in file e.g ASCII or numbers
Board createLevel1() {
    Board level(5, 5);
    for (int i = 0; i < level.columns(); ++i) {
        for (int j = 0; j < level.rows(); ++j) {
            int index = i * level.columns() + j;
            switch (index % 4) {
            case 0:
            case 1:
            case 2:
                ++totalPoints;
                level.getBox(i, j).setType(Box::Type::point);
                break;
            case 3:
                ++totalPoints;
                level.getBox(i, j).setType(Box::Type::super_point);
                break;
            }
        }
    }

    return level;
}

int main() {
    Game game;
    bool quit = false;
    SDL_Event e;
    Board board = createLevel1();

    SDL_Point start_pos = board.getPos();
    // TODO: change pacman start position to be inside box
    start_pos.x += (Box::size - Pacman::width) / 2;
    start_pos.y += (Box::size - Pacman::width) / 2;
    Pacman pacman(game.get_points_ref(), start_pos.x, start_pos.y);
    Ghost ghost(start_pos.x + (board.rows() - 1) * Box::size,
                start_pos.y + (board.columns() - 1) * Box::size);

    // TODO: create HUD class
    TextTexture points_texture;
    TextTexture fps_texture;
    TextTexture livesLeft_texture;
    std::ostringstream livesLeft_str;
    std::ostringstream points_str;
    std::ostringstream fps_str;
    SDL_Color black{0xFF, 0xFF, 0xFF, 0xFF};

    TextTexture theEndText;
    theEndText.loadText("The END", black);

    Timer fpsTimer;
    int frames = 0;
    fpsTimer.start();
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            pacman.handleEvent(e);
        }
        if (game.isEnd()) {
            SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
            SDL_RenderClear(Game::gRenderer);

            theEndText.render(
                Game::screen_width / 2 - theEndText.getWidth() / 2,
                Game::screen_height / 2 - theEndText.getHeight() / 2 -
                    points_texture.getHeight());
            points_texture.render(
                Game::screen_width / 2 - points_texture.getWidth() / 2,
                Game::screen_height / 2 - points_texture.getHeight() / 2);
            SDL_RenderPresent(Game::gRenderer);
            continue;
        }

        float avgFPS = frames / (fpsTimer.getTicks() / 1000.f);
        if (avgFPS > 2'000'000) {
            avgFPS = 0;
        }

        pacman.move(board);
        ghost.move(board, pacman);
        if (pacman.wasKilled()) {
            pacman.setLifesLeft(pacman.getLifesLeft() - 1);

            if (pacman.getLifesLeft() == 0) {
                game.setEnd();
                continue;
            }
            // TODO: set position (the map/level should know where pacman
            // and ghost start position is)
            int lives = pacman.getLifesLeft();
            // HACK: reset pacman and ghost position
            pacman = Pacman(game.get_points_ref(), start_pos.x, start_pos.y);
            pacman.setLifesLeft(lives);
            ghost = Ghost(start_pos.x + (board.rows() - 1) * Box::size,
                          start_pos.y + (board.columns() - 1) * Box::size);
        } else if (game.get_points() == totalPoints) {
            game.setEnd();
        }

        points_str.str("");
        points_str << "Points: " << game.get_points();
        fps_str.str("");
        fps_str << "FPS: " << static_cast<int>(avgFPS);
        // TODO: change this texture only when pacman is killed
        livesLeft_str.str("");
        livesLeft_str << "Lives: " << pacman.getLifesLeft();

        points_texture.loadText(points_str.str(), black);
        fps_texture.loadText(fps_str.str(), black);
        livesLeft_texture.loadText(livesLeft_str.str(), black);
        // clear screen
        SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(Game::gRenderer);

        board.render();
        points_texture.render(10, Game::screen_height -
                                      points_texture.getHeight() - 10);
        fps_texture.render(Game::screen_width - fps_texture.getWidth() - 10,
                           10);
        livesLeft_texture.render(
            Game::screen_width - livesLeft_texture.getWidth() - 10,
            Game::screen_height - livesLeft_texture.getHeight() - 10);

        pacman.render();
        ghost.render();

        // update screen
        SDL_RenderPresent(Game::gRenderer);
        ++frames;
    }
}
