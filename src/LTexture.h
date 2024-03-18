#ifndef LTEXTURE_H
#define LTEXTURE_H

#include <SDL2/SDL.h>
#include <string>
// Texture wrapper class
class LTexture {
  public:
    // Initializes variables
    LTexture();

    // Deallocates memory
    ~LTexture();

    // Loads image at specified path
    bool loadFromFile(const std::string& path);

    // Deallocates texture
    void free();

    // Renders texture at given point
    void render(int x, int y);
    void render(int x, int y, SDL_Rect *clip);
    void render(int x, int y, SDL_Rect &srcRect, SDL_Rect &clip);
    void render(int x, int y, SDL_Rect &srcRect, SDL_Rect &destRect,
                double angle);

    // Gets image dimensions
    int getWidth();
    int getHeight();

  private:
    // The actual hardware texture
    SDL_Texture *mTexture;

    // Image dimensions
    int mWidth;
    int mHeight;
};

#endif // !LTEXTURE_H
