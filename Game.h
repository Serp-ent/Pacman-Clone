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

    static constexpr int screen_width = 800;
    static constexpr int screen_height = 600;

    Game();

    void load_media();

    void close_game();

    ~Game() { close_game(); }

    void add_point() { ++points; }
    int get_points() const { return points; }
    int &get_points_ref() { return points; }

    bool isEnd() const { return end; }
    void setEnd(bool isEnd = true) { end = isEnd; }

  private:
    int points = 0;
    bool end = false;

    // points
    // map
    // board
    // player
};

#endif // !PACMAN_GAME_H
