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
    bool gameStarted = false;

    //***********************************************************************************

    int itemWidth = Game::screen_width / 2;
    int itemHeight = Game::screen_height / 8;
    int menuXpos = Game::screen_width / 2 - itemWidth / 2;

    std::unique_ptr<Menu> menu =
        std::make_unique<Menu>(menuXpos, 200, itemWidth, itemHeight);

    auto startGame = std::make_unique<StartGameAction>(gameStarted);

    MenuBuilder builder;

    builder.reset("settings");
    auto moveBack = std::make_unique<GoBackAction>(*menu);
    builder.addItem("Enemies", nullptr)
        .addItem("Volume", nullptr)
        .addItem("back", std::move(moveBack));
    auto settingsMenu = builder.build();
    auto openSettings =
        std::make_unique<OpenSubMenuAction>(*menu, std::move(settingsMenu));

    builder.reset("List levels");
    auto levelsGoBack = std::make_unique<GoBackAction>(*menu);
    for (int i = 0; i < 4; ++i) {
        std::string level = "level " + std::to_string(i);
        builder.addItem(level, nullptr);
    }
    builder.addItem("back", std::move(levelsGoBack));
    auto levelList = builder.build();
    auto listLevels =
        std::make_unique<OpenSubMenuAction>(*menu, std::move(levelList));

    builder.reset("High scores");
    auto highScoresBack = std::make_unique<GoBackAction>(*menu);

    builder.addItem("Marek: 200", nullptr)
        .addItem("Agata: 150", nullptr)
        .addItem("Szymon: 50", nullptr)
        .addItem("back", std::move(highScoresBack));
    auto listHighScores = builder.build();

    auto highScoresList =
        std::make_unique<OpenSubMenuAction>(*menu, std::move(listHighScores));

    auto quitGame = std::make_unique<QuitAction>(quit);

    builder.reset("Main Men");
    builder.addItem("Start", std::move(startGame))
        .addItem("Levels", std::move(listLevels))
        .addItem("High Scores", std::move(highScoresList))
        .addItem("Settings", std::move(openSettings))
        .addItem("Quit", std::move(quitGame));
    auto mainMenu = builder.build();

    menu->pushMenu(*mainMenu);

    //***********************************************************************************

    auto resumeGame = std::make_unique<StartGameAction>(isPaused);
    auto quitGamePause = std::make_unique<QuitAction>(quit);

    builder.reset("Pause Menu");
    builder.addItem("resume", std::move(resumeGame))
        .addItem("quit", std::move(quitGamePause));
    auto pauseMenu = builder.build();

    //***********************************************************************************

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    if (gameStarted) {
                        isPaused = !isPaused;
                        if (isPaused) {
                            menu->pushMenu(*pauseMenu);
                            // push pausedMenu
                        } else {
                            menu->popMenu();
                        }
                    }
                    break;
                case SDLK_UP:
                case SDLK_DOWN:
                case SDLK_RIGHT:
                case SDLK_LEFT:
                    pacman.start();
                    break;
                }
            } else if ((!gameStarted || isPaused) &&
                       e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (menu->handleMouse(mouseX, mouseY)) {
                    // BUG: when user presses resume game in paused state the
                    // fps jumps
                    fpsTimer.start(); // TODO: fps timer should be in some
                                      // screen class
                }

                if (gameStarted && !isPaused) {
                    while (!menu->empty()) {
                        menu->popMenu();
                    }
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
            menu->renderMenu();
            SDL_RenderPresent(Game::gRenderer);
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
