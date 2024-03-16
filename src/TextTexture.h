#ifndef PACMAN_TEXTTEXTURE_H
#define PACMAN_TEXTTEXTURE_H

#include <SDL2/SDL.h>
#include <string>

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

#endif // !PACMAN_TEXTTEXTURE_H
