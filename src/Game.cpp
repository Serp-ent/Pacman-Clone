#include "Game.h"

#include "Action.h"
#include "Board.h"
#include "Ghost.h"
#include "Hud.h"
#include "Menu.h"
#include "SDL2/SDL_image.h"
#include "TextTexture.h"

// TODO: check if we can have unique_ptr provided method
// that closes in dtor
SDL_Window *Game::gWindow = nullptr;
SDL_Renderer *Game::gRenderer = nullptr;
TTF_Font *Game::gFont = nullptr;

Game::Game() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("SDL could not initialize! SDL Error: %s\n",
                    SDL_GetError());
        throw std::runtime_error{"SDL could not initialize"};
    }

    Game::gWindow = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, screen_width,
                                     screen_height, SDL_WINDOW_SHOWN);
    if (!Game::gWindow) {
        std::printf("SDL could not create window! SDL Error: %s\n",
                    SDL_GetError());
        throw std::runtime_error{"SDL could not create window"};
    }

    // create renderer for window
    // TODO: check if we want to use VSYNC or just cap FPS
    Game::gRenderer = SDL_CreateRenderer(Game::gWindow, -1,
                                         SDL_RENDERER_ACCELERATED |
                                             SDL_RENDERER_PRESENTVSYNC);
    if (!Game::gRenderer) {
        std::printf("Renderer could not be created! SDL Error: %s\n",
                    SDL_GetError());
        throw std::runtime_error("Render could not be created");
    }

    // Initialize renderer color
    SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                    TTF_GetError());
        throw std::runtime_error("SDL_ttf could not initialize");
    }

    load_media();

    std::printf("Successfuly set up game\n");
}

void Game::load_media() {
    Game::gFont =
        TTF_OpenFont("./fonts/Oswald-VariableFont_wght.ttf", Box::size);
    if (!Game::gFont) {
        std::printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        throw std::runtime_error{"Failed to load font"};
    }

    //***************************************************

    if (!Pacman::sprite.loadFromFile("./textures/pacman_sprite.png")) {
        std::printf("Failed to load pacman sprite! SDL_ttf Error: %s\n",
                    IMG_GetError());
        throw std::runtime_error{"Failed to load pacman sprite"};
    }

    // TODO: create pacman sprite size instead of magic constant
    SDL_Point pos{1, 0};
    for (int i = 0; i < Pacman::frames; ++i) {
        Pacman::spriteClips[i] = {pos.x, pos.y, 16, 16};
        pos.x += 16;
    }

    // Full ball sprite
    Pacman::notStartedClip = {pos.x, pos.y, 16, 16};
    pos.x += 16;
    for (int i = 0; i < Pacman::deathFrames; ++i) {
        Pacman::deathSpriteClips[i] = {pos.x, pos.y, 16, 16};
        pos.x += 16;
    }

    pos = SDL_Point{1, 4 * 16 + 1};
    for (int i = 0; i < Ghost::frames; ++i) {
        Ghost::spriteClips[i] = {pos.x, pos.y, 16, 16};
        pos.x += 16;
    }
    for (int i = 0; i < Ghost::runningAwayFrames; ++i) {
        Ghost::runningAwayClips[i] = {pos.x, pos.y, 16, 16};
        pos.x += 16;
    }

    //***************************************************

    if (!Board::mapTexture.loadFromFile("./textures/map_sprite.png")) {
        std::printf("Failed to load map sprite! SDL_ttf Error: %s\n",
                    IMG_GetError());
        throw std::runtime_error{"Failed to load map sprite"};
    }

    pos = {0, 0};
    pos.y += 3 * (8 + 1); // read third sprite set
    for (int i = 0; i < Board::clipsNumber; ++i) {
        Board::mapClips[i] = {pos.x, pos.y, 8, 8};

        pos.x += 1 + 8;
        if ((i + 1) % 16 == 0) {
            pos.x = 0;
            pos.y = 1 + 8; // 1 means margin between textures
                           // 8 because textures are size 8px by 8px
        }
    }

    //***************************************************
}

void Game::close_game() {
    TTF_CloseFont(Game::gFont);

    SDL_DestroyRenderer(Game::gRenderer);
    SDL_DestroyWindow(Game::gWindow);

    // quit subsystems
    TTF_Quit();
    SDL_Quit();

    std::printf("Cleaning up game\n");
}

void Game::run() {
    SDL_Event e;
    Board board("./levels/level1.txt");

    Pacman pacman(get_points_ref(), board.getPacmanStart().x,
                  board.getPacmanStart().y);
    Ghost ghost(board.getGhostStart().x, board.getGhostStart().y);

    TextTexture theEndText;
    theEndText.loadText("Game over", white);

    Hud hud;

    //***********************************************************************************

    int itemWidth = Game::screen_width / 2;
    int itemHeight = Game::screen_height / 8;
    int menuXpos = Game::screen_width / 2 - itemWidth / 2;

    std::unique_ptr<Menu> menu =
        std::make_unique<Menu>(menuXpos, 200, itemWidth, itemHeight);

    auto startGame = std::make_unique<SetGameState>(*this, State::running);

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

    auto quitGame = std::make_unique<SetGameState>(*this, State::quit);

    builder.reset("Main Men");
    builder.addItem("Start", std::move(startGame))
        .addItem("Levels", std::move(listLevels))
        .addItem("High Scores", std::move(highScoresList))
        .addItem("Settings", std::move(openSettings))
        .addItem("Quit", std::move(quitGame));
    auto mainMenu = builder.build();

    menu->pushMenu(*mainMenu);

    //***********************************************************************************

    auto resumeGame = std::make_unique<SetGameState>(*this, State::running);
    auto quitGamePause = std::make_unique<SetGameState>(*this, State::quit);

    builder.reset("Pause Menu");
    builder.addItem("resume", std::move(resumeGame))
        .addItem("quit", std::move(quitGamePause));
    auto pauseMenu = builder.build();

    //***********************************************************************************

    // BUG: there is something wrong with path when ghost kills pacman
    // TODO: reset all entities to default state when pacman is killed
    while (state != State::quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                state = State::quit;

            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    if (state == State::running) {
                        state = State::paused;
                        menu->pushMenu(*pauseMenu);
                    } else if (state == State::paused) {
                        state = State::running;
                        menu->popMenu();
                    } else if (state == State::menu) {
                        if (menu->size() > 1) {
                            menu->popMenu();
                        }
                    } else {
                        printf("[ERROR]: unknown state on ESC click\n");
                    }
                    break;
                case SDLK_UP:
                case SDLK_DOWN:
                case SDLK_RIGHT:
                case SDLK_LEFT:
                    pacman.start();
                    break;
                }
            } else if ((state == State::menu || state == State::paused) &&
                       e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                menu->handleMouse(mouseX, mouseY);
            }

            if (!pacman.playsDeathAnimation()) {
                pacman.handleEvent(e);
            }
        }

        SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(Game::gRenderer);

        if (state == State::menu) {
            menu->renderMenu();
            SDL_RenderPresent(Game::gRenderer);

            continue;
        }

        if (state == State::paused) {
            menu->renderMenu();
            SDL_RenderPresent(Game::gRenderer);
            ++frames;
            continue;
        }

        if (state == State::end) {
            SDL_SetRenderDrawColor(Game::gRenderer, 0, 0, 0, 0xFF);
            SDL_RenderClear(Game::gRenderer);
            theEndText.render(
                Game::screen_width / 2 - theEndText.getWidth() / 2,
                Game::screen_height / 2 - theEndText.getHeight() / 2);

            SDL_RenderPresent(Game::gRenderer);
            continue;
        }

        hud.setPoints(get_points());
        hud.setFps(getAvgFps());
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
                state = State::end;
                continue;
            }
            int lives = pacman.getLifesLeft();
            pacman.setLifesLeft(lives);

            pacman.reset(board.getPacmanStart());
            ghost.reset(board.getGhostStart());
        } else if (get_points() == board.getTotalPoints()) {
            state = State::end;
        } else {
            pacman.move(board, ghost);
            ghost.move(board, pacman);
        }
    }
}
