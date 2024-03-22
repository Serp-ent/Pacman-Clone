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

    game.run();
}
