#ifndef PACMAN_HUD_H
#define PACMAN_HUD_H

#include "TextTexture.h"

class Hud {
  public:
    TextTexture points_texture;
    TextTexture fps_texture;
    TextTexture livesLeft_texture;

    void setPoints(int p) { points = p; }
    void setFps(int f) { fps = f; };
    void setLivesLeft(int l) { livesLeft = l; };

    void render();
    void refresh();

  private:
    int points = 0;
    int fps = 0;
    int livesLeft = 0;
};

#endif // !PACMAN_HUD_H
