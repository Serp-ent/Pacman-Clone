#ifndef PACMAN_BOX_H
#define PACMAN_BOX_H

#include "Pacman.h"

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

#endif // !PACMAN_BOX_H
