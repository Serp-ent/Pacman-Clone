#ifndef PACMAN_HUD_H
#define PACMAN_HUD_H

#include "Pacman.h"
#include "TextTexture.h"
#include "namedColors.h"
#include <SDL2/SDL_rect.h>

class Hud {
  public:
    Hud() { paused_texture.loadText("Game Paused", white); }

    void setPoints(int p) { points = p; }
    void setFps(int f) { fps = f; };
    void setLivesLeft(int l) { livesLeft = l; };

    void gamePaused();

    void render();
    void refresh();

  private:
    TextTexture points_texture;
    TextTexture fps_texture;
    TextTexture paused_texture;

    const SDL_Rect *pacman_texture = &Pacman::spriteClips[0];

    int points = 0;
    int fps = 0;
    int livesLeft = 0;
};

#endif // !PACMAN_HUD_H
