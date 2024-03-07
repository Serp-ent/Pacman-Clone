#ifndef PACMAN_TIMER_H
#define PACMAN_TIMER_H

#include <SDL2/SDL.h>

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


#endif // !PACMAN_TIMER_H
