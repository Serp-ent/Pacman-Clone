#include "Action.h"
#include "Board.h"
#include "Box.h"
#include "Ghost.h"
#include "Hud.h"
#include "Menu.h"
#include "Pacman.h"
#include "TextTexture.h"
#include "Timer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <memory>
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
    bool isPaused = false;
    constexpr int padding = 5;

    bool gameStarted = false;

    //***********************************************************************************
    // TODO: builder pattern to building menu

    std::unique_ptr<Menu> menu = std::make_unique<Menu>();

    auto settingsMenu = std::make_unique<MenuBox>("settings", nullptr);

    auto moveBack = std::make_unique<GoBackAction>(*menu);
    settingsMenu->addItem("Back", std::move(moveBack));

    auto openSettings =
        std::make_unique<OpenSubMenuAction>(*menu, std::move(settingsMenu));
    auto startGame = std::make_unique<StartGameAction>(gameStarted);
    auto quitGame = std::make_unique<QuitAction>(quit);

    auto mainMenu = std::make_unique<MenuBox>("MainMenu", nullptr);
    mainMenu->addItem("Start", std::move(startGame));
    mainMenu->addItem("Settings", std::move(openSettings));

    mainMenu->addItem("Quit", std::move(quitGame));

    menu->pushMenu(*mainMenu);

    //***********************************************************************************

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    isPaused = !isPaused;
                    break;
                case SDLK_UP:
                case SDLK_DOWN:
                case SDLK_RIGHT:
                case SDLK_LEFT:
                    pacman.start();
                    break;
                }
            } else if (!gameStarted && e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (menu->handleMouse(mouseX, mouseY)) {
                    printf("Menu::start clicked\n");
                    fpsTimer.start(); // TODO: fps timer should be in some
                                      // screen class
                }
            }

            if (!pacman.playsDeathAnimation()) {
                pacman.handleEvent(e);
            }
        }

        SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(Game::gRenderer);

        if (!gameStarted) {
            menu->renderMenu();
            SDL_RenderPresent(Game::gRenderer);

            continue;
        }

        if (isPaused) {
            hud.gamePaused();
            ++frames;
            continue;
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
