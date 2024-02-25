#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <cstdio>
#include <vector>

// TODO: use SDL functions with unique_ptr
SDL_Window *gWindow;
SDL_Renderer *gRenderer;

constexpr int screen_width = 800;
constexpr int screen_height = 600;

class Pacman {
  public:
    static constexpr int height = 40;
    static constexpr int width = 40;
    static constexpr int velocity = 1;

    Pacman() : texture({0, 0, width, height}), velocity_x{0}, velocity_y{0} {}

    void handleEvent(SDL_Event &e);
    void move();
    void render() const;

  private:
    // TODO: add sprite
    SDL_Rect texture;

    int velocity_x;
    int velocity_y;
};

class Box {
  public:
    static constexpr int size = Pacman::height;
    enum class Type : int8_t { wall, point, super_point, empty };

    void render(int x, int y);
    void setType(Box::Type t) { type_ = t; };
    Type getType() const { return type_; }

  private:
    Type type_;
};

void Box::render(int x, int y) {
    SDL_Rect r{x, y, size, size};

    switch (type_) {
    case Type::super_point:
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0xFF);
        break;
    case Type::wall:
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
        break;
    case Type::point:
        SDL_SetRenderDrawColor(gRenderer, 0, 0xFF, 0, 0xFF);
        break;
    case Type::empty:
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xF);
        break;
    }

    SDL_RenderFillRect(gRenderer, &r);
};

class Board {
  public:
    Board(int w, int h) : rows_(h), columns_(h), board_(w * h) {
        for (int i = 0; i < board_.size(); ++i) {
            switch (i % 4) {
            case 0:
                board_[i].setType(Box::Type::wall);
                break;
            case 1:
                board_[i].setType(Box::Type::empty);
                break;
            case 2:
                board_[i].setType(Box::Type::point);
                break;
            case 3:
                board_[i].setType(Box::Type::super_point);
                break;
            }
        }
    }

    void render();

    int rows() const { return rows_; }
    int column() const { return columns_; }
    Box &getBox(int i, int j) { return board_.at(i * rows() + j); }

  private:
    const int rows_;
    const int columns_;

    std::vector<Box> board_;
};

void Board::render() {
    // TODO: add padding
    // so the board will be in the center of window
    for (int i = 0; i < rows(); ++i) {
        for (int j = 0; j < column(); ++j) {
            getBox(i, j).render(i * Box::size, j * Box::size);
        }
    }
}

void Pacman::move() {
    texture.x += velocity_x;
    if ((texture.x < 0) || (texture.x + texture.w > screen_width)) {
        texture.x -= velocity_x;
    }

    texture.y += velocity_y;
    if ((texture.y < 0) || (texture.y + texture.h) > screen_height) {
        texture.y -= velocity_y;
    }
}

void Pacman::handleEvent(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
            velocity_y -= velocity;
            break;
        case SDLK_DOWN:
            velocity_y += velocity;
            break;
        case SDLK_RIGHT:
            velocity_x += velocity;
            break;
        case SDLK_LEFT:
            velocity_x -= velocity;
            break;
        }
    } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
            velocity_y += velocity;
            break;
        case SDLK_DOWN:
            velocity_y -= velocity;
            break;
        case SDLK_RIGHT:
            velocity_x -= velocity;
            break;
        case SDLK_LEFT:
            velocity_x += velocity;
            break;
        }
    }
}

void Pacman::render() const {
    // more rendering
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0, 0xFF);
    SDL_RenderFillRect(gRenderer, &texture);

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
    SDL_RenderDrawRect(gRenderer, &texture);
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

    // Create window

    return success;
}

void close_game() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
}

int main() {
    if (!init_game()) {
        std::printf("Failed to initialize!\n");
        return 1;
    }

    std::printf("Successfuly set up game\n");
    bool quit = false;
    SDL_Event e;
    Pacman pacman;
    Board board(14, 14);
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            pacman.handleEvent(e);
        }
        pacman.move();

        // clear screen
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        board.render();
        pacman.render();

        // update screen
        SDL_RenderPresent(gRenderer);
    }

    std::printf("Cleaning up game\n");

    close_game();
}
