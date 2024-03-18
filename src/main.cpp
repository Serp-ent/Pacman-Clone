#include "Board.h"
#include "Box.h"
#include "Ghost.h"
#include "Hud.h"
#include "Pacman.h"
#include "TextTexture.h"
#include "Timer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
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

int main() {
    Game game;
    bool quit = false;
    SDL_Event e;
    Board board("./levels/level1.txt");

    Pacman pacman(game.get_points_ref(), board.getPacmanStart().x,
                  board.getPacmanStart().y);

    Ghost ghost(board.getGhostStart().x, board.getGhostStart().y);

    // TODO: create HUD class
    SDL_Color black{0xFF, 0xFF, 0xFF, 0xFF};

    TextTexture theEndText;
    theEndText.loadText("Game over", black);

    Hud hud;
    Timer fpsTimer;
    int frames = 0;
    fpsTimer.start();
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_DOWN:
                case SDLK_RIGHT:
                case SDLK_LEFT:
                    pacman.start();
                    break;
                }
            }

            if (!pacman.playsDeathAnimation()) {
                pacman.handleEvent(e);
            }
        }

        if (game.isEnd()) {
            SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
            SDL_RenderClear(Game::gRenderer);
            theEndText.render(
                Game::screen_width / 2 - theEndText.getWidth() / 2,
                Game::screen_height / 2 - theEndText.getHeight() / 2);

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

        // TODO: create some graphic class
        hud.render();

        board.render();

        pacman.render();
        ghost.render();

        // update screen
        SDL_RenderPresent(Game::gRenderer);
        ++frames;

        if (!pacman.isStarted()) {
            continue;
        }

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
            pacman.setPos(board.getPacmanStart());
            pacman.setLifesLeft(lives);

            ghost.setPos(board.getGhostStart());

            pacman.stop();
        } else if (game.get_points() == board.getTotalPoints()) {
            game.setEnd();
        } else {
            pacman.move(board, ghost);
            ghost.move(board, pacman);
        }
    }
}
