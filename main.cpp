#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

// TODO: use SDL functions with unique_ptr
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
TTF_Font *gFont = nullptr;

constexpr int screen_width = 800;
constexpr int screen_height = 600;

int points = 0;
int totalPoints = 0;

class Board;

class TextTexture {
  public:
    TextTexture() : texture(nullptr), width(0), height(0){};
    ~TextTexture() { free(); };

    bool loadText(const std::string &text, SDL_Color color);
    void free();
    void set_color(Uint8 r, Uint8 g, Uint8 b);

    void render(int x, int y);

    int getWidth() const { return width; };
    int getHeight() const { return height; };

  private:
    SDL_Texture *texture;

    int width;
    int height;
};

void TextTexture::free() {
    if (!texture) {
        return;
    }

    SDL_DestroyTexture(texture);
    texture = nullptr;
    width = height = 0;
}
void TextTexture::set_color(Uint8 r, Uint8 g, Uint8 b) {}

void TextTexture::render(int x, int y) {
    SDL_Rect render = {x, y, width, height};
    SDL_RenderCopy(gRenderer, texture, NULL, &render);
}

bool TextTexture::loadText(const std::string &text, SDL_Color color) {
    // get rid of preexisintg texture
    free();

    SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, text.c_str(), color);
    if (!textSurface) {
        std::printf("Unable to render text surface! SDL_ttf Error: %s\n",
                    TTF_GetError());
        return false;
    }

    // create texture from surface pixels
    bool success = true;
    texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (!texture) {
        std::printf(
            "Unable to create texture from rendered text! SDL Error: %s\n",
            SDL_GetError());
        success = false;
    } else {
        // get image dimensions
        width = textSurface->w;
        height = textSurface->h;
    }

    SDL_FreeSurface(textSurface);

    return success;
}

class Timer {
  public:
    Timer() : startTicks{0}, running{false} {}

    void start() {
        running = true;
        paused = false;

        startTicks = SDL_GetTicks();
        pausedTicks = 0;
    }

    void stop() {
        running = false;
        paused = false;

        pausedTicks = 0;
        startTicks = 0;
    }

    void pause() {
        if (running && !paused) {
            paused = true;
            // calculate the paused ticks
            pausedTicks = SDL_GetTicks() - startTicks;
            startTicks = 0;
        }
    }

    void unpause() {
        if (running && paused) {
            paused = false;
            startTicks = SDL_GetTicks() - pausedTicks;
            pausedTicks = 0;
        }
    }

    Uint32 getTicks() const {
        Uint32 time = 0;
        if (running) {
            if (paused) {
                time = pausedTicks;
            } else {
                time = SDL_GetTicks() - startTicks;
            }
        }

        return time;
    }

    bool isRunning() const { return running; }
    bool isPaused() const { return paused; }

  private:
    Uint32 startTicks;  // when clock time started
    Uint32 pausedTicks; // ticks stored when the timer was paused

    bool running;
    bool paused;
};

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
          board_(w * h) {}

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

Board createLevel1() {
    Board level(14, 14);
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
                level.getBox(i, j).setType(Box::Type::wall);
                break;
            }
        }
    }

    return level;
}

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

Box *pointIsReached(SDL_Rect &p, Board &b, Box::Type boxType) {
    // TODO: check only surrounding boxes

    SDL_Rect box{b.getPos().x, b.getPos().y, Box::size, Box::size};
    for (int i = 0; i < b.rows(); ++i) {
        for (int j = 0; j < b.columns(); ++j) {
            SDL_Point box_center{box.x + Box::size / 2, box.y + Box::size / 2};
            if (b.getBox(i, j).getType() == boxType &&
                SDL_PointInRect(&box_center, &p)) {

                return &b.getBox(i, j);
            }

            box.x += Box::size;
        }

        box.x = b.getPos().x;
        box.y += Box::size;
    }

    return nullptr;
}

void Pacman::move(Board &b) {
    SDL_Rect border{b.getPos().x, b.getPos().y, b.columns() * Box::size,
                    b.rows() * Box::size};

    // TODO: fix jumping
    // TODO: fix moving inside on rectnagle when there is border
    // e.g. pacman can move up and down when there are upper and lower boxes
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

    Box *box;
    if ((box = pointIsReached(texture, b, Box::Type::point)) != nullptr) {
        ++points;
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

    TextTexture points_texture;
    TextTexture fps_texture;
    std::ostringstream points_str;
    std::ostringstream fps_str;
    SDL_Color black{0xFF, 0xFF, 0xFF, 0xFF};

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

        float avgFPS = frames / (fpsTimer.getTicks() / 1000.f);
        if (avgFPS > 2'000'000) {
            avgFPS = 0;
        }

        pacman.move(board);

        points_str.str("");
        points_str << "Points: " << points;
        fps_str.str("");
        fps_str << "FPS: " << static_cast<int>(avgFPS);

        points_texture.loadText(points_str.str(), black);
        fps_texture.loadText(fps_str.str(), black);
        // clear screen
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(gRenderer);

        board.render();
        points_texture.render(10,
                              screen_height - points_texture.getHeight() - 10);
        fps_texture.render(screen_width - fps_texture.getWidth() - 10, 10);

        pacman.render();

        if (points == totalPoints) {
            std::printf("You won\n");
            break;
        }

        // update screen
        SDL_RenderPresent(gRenderer);
        ++frames;
    }

    std::printf("Cleaning up game\n");

    close_game();
}
