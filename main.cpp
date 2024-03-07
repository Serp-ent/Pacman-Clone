#include "Board.h"
#include "Box.h"
#include "Pacman.h"
#include "TextTexture.h"
#include "Timer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// TODO: create MENU and HUD

// TODO: create level designer in SDL2 that creates textfile with map
// TODO: use SDL functions with unique_ptr
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
TTF_Font *gFont = nullptr;

constexpr int screen_width = 800;
constexpr int screen_height = 600;

bool checkCollision(const SDL_Rect &a, const SDL_Rect &b);
int points = 0;
int totalPoints = 0;


// TODO: inherit from Enitty object
// create GHOST enemy

class Ghost {
    // TODO: set random point that Ghost travel to
  public:
    static constexpr int height = 30;
    static constexpr int width = 30;
    static constexpr int velocity = 2;

    Ghost() : texture({0, 0, width, height}), velocity_x{0}, velocity_y{0} {}
    Ghost(int x, int y)
        : texture({x, y, width, height}), velocity_x{0}, velocity_y{0} {}

    void move(Board &b, Pacman &pacman);
    void render() const;

  private:
    // TODO: add sprite
    SDL_Rect texture;

    int velocity_x;
    int velocity_y;
};

void Ghost::render() const {
    // more rendering
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0xAA);
    SDL_RenderFillRect(gRenderer, &texture);

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
    SDL_RenderDrawRect(gRenderer, &texture);
}

// - wall -> block / collision
// - point -> gain 10 points
// - super-point -> gain 100 points and ability to eat ghosts
// - empty -> pacman can go through

Box *pointIsReached(SDL_Rect &p, Board &b, Box::Type boxType);
Box *checkCollision(SDL_Rect &p, Board &b, Box::Type boxType);

void Ghost::move(Board &b, Pacman &pacman) {

    // TODO: non-random decisions
    static int moveNumber = 0;
    if (moveNumber == 40) {
        int direction = random();
        velocity_y = 0;
        velocity_x = 0;
        switch (direction % 4) {
        case 0:
            velocity_x += 2;
            break;
        case 1:
            velocity_x -= 2;
            break;
        case 2:
            velocity_y -= 2;
            break;
        case 3:
            velocity_y += 2;
            break;
        }

        moveNumber = 0;
    }

    ++moveNumber;

    SDL_Rect border{b.getPos().x, b.getPos().y, b.columns() * Box::size,
                    b.rows() * Box::size};

    // TODO: fix jumping
    // TODO: fix moving inside on rectnagle when there is border
    // e.g. can move up and down when there are upper and lower boxes
    // // maybe keep surrounding 9 box that surrounds pacman
    // and check if we can enter them
    SDL_Point texture_center = {texture.x + Pacman::width / 2,
                                texture.y + Pacman::height / 2};

    int i = (texture_center.x - b.getPos().x) / Box::size;
    int j = (texture_center.y - b.getPos().y) / Box::size;
    if (velocity_x) {
        texture.x += velocity_x;
        // fix y position
        SDL_Point curr_box_center = {
            b.getPos().x + i * Box::size + Box::size / 2,
            b.getPos().y + j * Box::size + Box::size / 2};
        int correct_y = curr_box_center.y - texture.h / 2;
        texture.y = correct_y;

        if ((texture.x < border.x) ||
            (texture.x + texture.w > border.x + border.w) ||
            checkCollision(texture, b, Box::Type::wall)) {
            texture.x -= velocity_x;
        }
    } else if (velocity_y) {
        texture.y += velocity_y;

        // fix x position
        SDL_Point curr_box_center = {
            b.getPos().x + i * Box::size + Box::size / 2,
            b.getPos().y + j * Box::size + Box::size / 2};
        int correct_x = curr_box_center.x - texture.w / 2;
        texture.x = correct_x;

        if ((texture.y < border.y) ||
            (texture.y + texture.h) > border.y + border.h ||
            checkCollision(texture, b, Box::Type::wall)) {
            texture.y -= velocity_y;
        }
    }

    // Make collision only point so textures can overlap
    SDL_Point collison{texture.x + texture.w / 2, texture.y + texture.h / 2};
    if (SDL_PointInRect(&collison, &pacman.getCollision())) {
        pacman.clearState(true);
    }
}

// TODO: don't return by copy
Board createLevel1() {
    Board level(5, 5);
    for (int i = 0; i < level.columns(); ++i) {
        for (int j = 0; j < level.rows(); ++j) {
            int index = i * level.columns() + j;
            switch (index % 4) {
            case 0:
            case 1:
            case 2:
            case 3:
                ++totalPoints;
                level.getBox(i, j).setType(Box::Type::point);
                break;
            case 4:
                // BUG: non-reachable
                level.getBox(i, j).setType(Box::Type::wall);
                break;
            }
        }
    }

    return level;
}

bool init_game() {
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("SDL could not initialize! SDL Error: %s\n",
                    SDL_GetError());
        return false;
    }
    gWindow = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, screen_width,
                               screen_height, SDL_WINDOW_SHOWN);
    if (!gWindow) {
        std::printf("SDL could not create window! SDL Error: %s\n",
                    SDL_GetError());
        return false;
    }

    // create renderer for window
    // TODO: check if we want to use VSYNC or just cap FPS
    gRenderer = SDL_CreateRenderer(
        gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!gRenderer) {
        std::printf("Renderer could not be created! SDL Error: %s\n",
                    SDL_GetError());
        return false;
    }

    // Initialize renderer color
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                    TTF_GetError());
        success = false;
    }

    // Create window

    return success;
}

bool load_media() {
    bool success = true;

    gFont = TTF_OpenFont("./Oswald-VariableFont_wght.ttf", Box::size / 2);
    if (!gFont) {
        std::printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }

    return success;
}

void close_game() {
    TTF_CloseFont(gFont);

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    // quit subsystems
    TTF_Quit();
    SDL_Quit();
}

int main() {
    if (!init_game()) {
        std::printf("Failed to initialize!\n");
        return 1;
    }

    if (!load_media()) {
        std::printf("Failed to load media!\n");
        return 1;
    }

    std::printf("Successfuly set up game\n");
    bool quit = false;
    SDL_Event e;
    Board board = createLevel1();

    SDL_Point start_pos = board.getPos();
    // TODO: change pacman start position to be inside box
    start_pos.x += (Box::size - Pacman::width) / 2;
    start_pos.y += (Box::size - Pacman::width) / 2;
    Pacman pacman(start_pos.x, start_pos.y);
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
    bool gameEnd = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            pacman.handleEvent(e);
        }
        if (gameEnd) {
            SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
            SDL_RenderClear(gRenderer);

            theEndText.render(screen_width / 2 - theEndText.getWidth() / 2,
                              screen_height / 2 - theEndText.getHeight() / 2 -
                                  points_texture.getHeight());
            points_texture.render(
                screen_width / 2 - points_texture.getWidth() / 2,
                screen_height / 2 - points_texture.getHeight() / 2);
            SDL_RenderPresent(gRenderer);
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
                gameEnd = true;
                continue;
            }
            // TODO: set position (the map/level should know where pacman
            // and ghost start position is)
            int lives = pacman.getLifesLeft();
            // HACK: reset pacman and ghost position
            pacman = Pacman(start_pos.x, start_pos.y);
            pacman.setLifesLeft(lives);
            ghost = Ghost(start_pos.x + (board.rows() - 1) * Box::size,
                          start_pos.y + (board.columns() - 1) * Box::size);
        }

        points_str.str("");
        points_str << "Points: " << points;
        fps_str.str("");
        fps_str << "FPS: " << static_cast<int>(avgFPS);
        // TODO: change this texture only when pacman is killed
        livesLeft_str.str("");
        livesLeft_str << "Lives: " << pacman.getLifesLeft();

        points_texture.loadText(points_str.str(), black);
        fps_texture.loadText(fps_str.str(), black);
        livesLeft_texture.loadText(livesLeft_str.str(), black);
        // clear screen
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(gRenderer);

        board.render();
        points_texture.render(10,
                              screen_height - points_texture.getHeight() - 10);
        fps_texture.render(screen_width - fps_texture.getWidth() - 10, 10);
        livesLeft_texture.render(
            screen_width - livesLeft_texture.getWidth() - 10,
            screen_height - livesLeft_texture.getHeight() - 10);

        pacman.render();
        ghost.render();

        if (points == totalPoints) {
            gameEnd = true;
        }

        // update screen
        SDL_RenderPresent(gRenderer);
        ++frames;
    }

    std::printf("Cleaning up game\n");

    close_game();
}
