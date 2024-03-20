#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "Game.h"
#include "TextTexture.h"
#include "namedColors.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <memory>
#include <string>
#include <vector>

class Action {
  public:
    virtual ~Action() {}

    virtual void execute() = 0;
};

class OpenSettingsAction : public Action {
  public:
    OpenSettingsAction() {}

    void execute() override { printf("Open settings submenu\n"); }
};

class QuitAction : public Action {
  public:
    QuitAction(bool &flag) : end{flag} {}
    void execute() override { end = true; }

  private:
    bool &end;
};

class StartGameAction : public Action {
  public:
    StartGameAction(bool &flag) : gameStarted{flag} {}

    void execute() override { gameStarted = true; }

  private:
    bool &gameStarted;
};

//******************************************************************************

struct MenuItem {
  public:
    MenuItem(const std::string &t, int x, int y, int w, int h,
             std::unique_ptr<Action> a)
        : rect{x, y, w, h}, action{std::move(a)} {
        text.loadText(t, white);
    }

    void render() {
        text.render(rect.x + rect.w / 2 - text.getWidth() / 2,
                    rect.y + rect.h / 2 - text.getHeight() / 2);

        SDL_SetRenderDrawColor(Game::gRenderer, 255, 0, 0, 0xFF);
        SDL_RenderDrawRect(Game::gRenderer, &rect);
    }

    bool isMouseOver(int mouseX, int mouseY) {
        // TODO: use a SDL_inRect function or something like that
        if (rect.x > mouseX || rect.x + rect.w < mouseX) {
            return false;
        }
        if (rect.y > mouseY || rect.y + rect.h < mouseY) {
            return false;
        }

        return true;
    }

    void invokeAction() { action->execute(); }

  private:
    std::unique_ptr<Action> action;

    SDL_Rect rect;
    TextTexture text;
};

class Menu {
  public:
    Menu(int x, int y, int w, int h)
        : topleft{x, y}, itemWidth(w), itemHeight(h) {}

    void addItem(const std::string &name, std::unique_ptr<Action> action) {
        int startX = topleft.x;
        int startY = topleft.y + items.size() * (itemHeight + padding);

        auto item = std::make_unique<MenuItem>(name, startX, startY, itemWidth,
                                               itemHeight, std::move(action));
        items.push_back(std::move(item));
    }

    void setPadding(int p) { padding = p; }

    void render() {
        for (auto &i : items) {
            i->render();
        }
    }

    bool handleMouse(int mouseX, int mouseY) {
        int which = 0;
        for (auto &i : items) {
            if (i->isMouseOver(mouseX, mouseY)) {
                i->invokeAction();
                return true;
            }

            ++which;
        }
        return false;
    }

  private:
    // BUG: should be MenuItem instead of pointers but it doesn't work when
    // having values
    std::vector<std::unique_ptr<MenuItem>> items;
    SDL_Point topleft;

    int padding = 0;
    int itemWidth;
    int itemHeight;
};

#endif // !GAME_MENU_H
