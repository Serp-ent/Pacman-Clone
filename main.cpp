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

class Board;

// TODO: inherit from Enitty object
// create GHOST enemy

class Pacman {
  public:
    static constexpr int height = 30;
    static constexpr int width = 30;
    static constexpr int velocity = 2;

    Pacman() : texture({0, 0, width, height}), velocity_x{0}, velocity_y{0} {}
    Pacman(int x, int y)
        : texture({x, y, width, height}), velocity_x{0}, velocity_y{0} {}

    void handleEvent(SDL_Event &e);
    void move(Board &b);
    void render() const;

  private:
    // TODO: add sprite
    SDL_Rect texture;

    int velocity_x;
    int velocity_y;
};

// - wall -> block / collision
// - point -> gain 10 points
// - super-point -> gain 100 points and ability to eat ghosts
// - empty -> pacman can go through
class Box {
  public:
    static constexpr int size = Pacman::height + 10;
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
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0, 0xFF);
        break;
    case Type::empty:
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xF);
        break;
    }

    SDL_RenderFillRect(gRenderer, &r);
};

class Board {
  public:
    Board(int w, int h)
        : rows_(h), columns_(h),
          border{screen_width / 2 - (rows() * Box::size) / 2,
                 screen_height / 2 - (columns() * Box::size) / 2,
                 columns() * Box::size, rows() * Box::size},
          board_(w * h) {
        for (int i = 0; i < board_.size(); ++i) {
            switch (i % 4) {
            case 0:
            case 1:
            case 2:
                board_[i].setType(Box::Type::point);
                break;
            case 3:
                board_[i].setType(Box::Type::wall);
                break;
            }
        }
    }

    SDL_Point getPos() const {
        SDL_Point p{border.x, border.y};
        return p;
    }

    void render();

    int rows() const { return rows_; }
    int columns() const { return columns_; }
    Box &getBox(int i, int j) { return board_.at(i * rows() + j); }

  private:
    const int rows_;
    const int columns_;

    const SDL_Rect border;

    std::vector<Box> board_;
};

void Board::render() {
    SDL_Point box{border.x, border.y};
    for (int i = 0; i < rows(); ++i) {
        for (int j = 0; j < columns(); ++j) {
            getBox(i, j).render(box.x, box.y);
            box.x += Box::size;
        }
        box.x = border.x;
        box.y += Box::size;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRect(gRenderer, &border);
}

bool checkCollision(const SDL_Rect &a, const SDL_Rect &b) {
    // The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    // Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    // Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;
    // If any of the sides from A are outside of B
    if (bottomA <= topB) {
        return false;
    }

    if (topA >= bottomB) {
        return false;
    }

    if (rightA <= leftB) {
        return false;
    }

    if (leftA >= rightB) {
        return false;
    }

    // If none of the sides from A are outside B
    return true;
}

Box *checkCollision(SDL_Rect &p, Board &b, Box::Type boxType) {
    // TODO: check only surrounding boxes
    SDL_Rect box{b.getPos().x, b.getPos().y, Box::size, Box::size};
    for (int i = 0; i < b.rows(); ++i) {
        for (int j = 0; j < b.columns(); ++j) {
            if (b.getBox(i, j).getType() == boxType && checkCollision(p, box)) {
                return &b.getBox(i, j);
            }

            box.x += Box::size;
        }

        box.x = b.getPos().x;
        box.y += Box::size;
    }

    return nullptr;
    // collision boxes
}

void Pacman::move(Board &b) {
    SDL_Rect border{b.getPos().x, b.getPos().y, b.columns() * Box::size,
                    b.rows() * Box::size};

    texture.x += velocity_x;
    if ((texture.x < border.x) ||
        (texture.x + texture.w > border.x + border.w) ||
        checkCollision(texture, b, Box::Type::wall)) {
        texture.x -= velocity_x;
    }

    texture.y += velocity_y;
    if ((texture.y < border.y) ||
        (texture.y + texture.h) > border.y + border.h ||
        checkCollision(texture, b, Box::Type::wall)) {
        texture.y -= velocity_y;
    }

    Box *box;
    if ((box = checkCollision(texture, b, Box::Type::point)) != nullptr) {
        // TODO: emmit sound for eating ball point
        box->setType(Box::Type::empty);
    }
}

void Pacman::handleEvent(SDL_Event &e) {
    // TODO: pacman should not change direction when there is collison
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
            velocity_y = -velocity;
            velocity_x = 0;
            break;
        case SDLK_DOWN:
            velocity_y = velocity;
            velocity_x = 0;
            break;
        case SDLK_RIGHT:
            velocity_y = 0;
            velocity_x = velocity;
            break;
        case SDLK_LEFT:
            velocity_y = 0;
            velocity_x = -velocity;
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
    Board board(14, 14);

    SDL_Point start_pos = board.getPos();
    Pacman pacman(start_pos.x, start_pos.y);
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            pacman.handleEvent(e);
        }
        pacman.move(board);

        // clear screen
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(gRenderer);

        board.render();
        pacman.render();

        // update screen
        SDL_RenderPresent(gRenderer);
    }

    std::printf("Cleaning up game\n");

    close_game();
}
