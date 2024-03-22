#ifndef PACMAN_GAME_H
#define PACMAN_GAME_H

#include "Box.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <stdexcept>

class Game {
  public:
    static SDL_Window *gWindow;
    static SDL_Renderer *gRenderer;
    static TTF_Font *gFont;

    static constexpr int screen_width = 1280;
    static constexpr int screen_height = 960;

    enum class State : uint8_t {
        menu,
        notStarted,
        running,
        paused,
        end,
        quit,
    };

    Game();

    void load_media();

    void close_game();

    void run();

    ~Game() { close_game(); }

    void add_point() { ++points; }
    int get_points() const { return points; }
    int &get_points_ref() { return points; }

    State getState() { return state; }
    void setState(State s) { state = s; }

    void resetFps() {
        frames = 0;
        fpsTimer.start();
    }

    int getAvgFps() {
        float avgFPS = frames / (fpsTimer.getTicks() / 1000.f);
        if (avgFPS > 2'000'000) {
            avgFPS = 0;
        }
        return static_cast<int>(avgFPS);
    }

  private:
    int points = 0;
    int frames = 0;
    Timer fpsTimer;

    // TODO: these 3 should be in one enum class
    State state;

    // points
    // map
    // board
    // player
};

#endif // !PACMAN_GAME_H
