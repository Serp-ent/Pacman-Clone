#include "Board.h"
#include "Game.h"

void Board::render() {
    SDL_Point box{border.x, border.y};
    for (int i = 0; i < rows(); ++i) {
        for (int j = 0; j < columns(); ++j) {
            getBox(i, j).render(box.x, box.y);
            box.x += Box::size;
        }
        box.x = border.x;
        box.y += Box::size;
    }

    SDL_SetRenderDrawColor(Game::gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRect(Game::gRenderer, &border);
}
