#ifndef PACMAN_BOX_H
#define PACMAN_BOX_H

#include "Ghost.h"
#include "Pacman.h"
#include <SDL2/SDL_rect.h>

class Box {
  public:
    static constexpr int size = Pacman::height;
    enum class Type : int8_t { wall, point, super_point, empty };
    enum WallType {
        MapTopRightCorner,
        MapTopLeftCorner,
        MapRightWall,
        MapLeftWall,
        MapBottomRightCorner,
        MapBottomLeftCorner,
        MapConcaveBottomRight,
        MapConcaveBottomLeft,
        MapConcaveTopRight,
        MapConcaveTopLeft,
        Junk5,
        MapTopWall,
        MapTopWall_v2, // ???
        MapBottomWall,
        MapBottomWall_v2, // ???
        BottomWall,
        BottomWall_v2, // ???
        Junk9,
        Junk10,
        Junk11,
        TopWall,
        TopWall_v2, // ???
        ConcaveTopRight,
        ConcaveTopLeft,
        LeftWall,
        RightWall,
        ConcaveBottomLeft,
        ConcaveBottomRight,
        Junk15,
        Junk21,
        Junk22,
        Junk23,
        Junk24,
        Junk25,
        TopLeftCorner,
        TopRightCorner,
        BottomLeftCorner,
        BottomRightCorner,
        Junk30,
        Junk31,
        Junk32,
        Junk33,
        Junk34,
        Junk35,
        Junk36,
        Junk37,
        InsideWall, // surrouned by walls
    };

    void render(int x, int y);
    void setType(Box::Type t) { type_ = t; };
    Type getType() const { return type_; }

    void setClip(SDL_Rect *clip) { currClip = clip; }
    Box::WallType getClip();

  private:
    SDL_Rect *currClip = nullptr;
    Type type_;
};

#endif // !PACMAN_BOX_H
