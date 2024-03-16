#ifndef PACMAN_BOX_H
#define PACMAN_BOX_H

#include "Ghost.h"
#include "Pacman.h"
#include <SDL2/SDL_rect.h>

class Box {
  public:
    static constexpr int size = Pacman::height;
    enum class Type : int8_t { wall, point, super_point, empty };

    void render(int x, int y);
    void setType(Box::Type t) { type_ = t; };
    Type getType() const { return type_; }

    void setClip(SDL_Rect *clip) { currClip = clip; }

  private:
    SDL_Rect *currClip = nullptr;
    Type type_;
};

#endif // !PACMAN_BOX_H
