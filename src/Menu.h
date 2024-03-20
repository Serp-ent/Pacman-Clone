#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "Game.h"
#include "TextTexture.h"
#include "namedColors.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <memory>
#include <stack>
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

class OpenSubMenuAction : public Action {
  public:
  private:
};

//******************************************************************************

class MenuItem {
  public:
    MenuItem(const std::string &label, std::unique_ptr<Action> a)
        : action{std::move(a)} {
        text.loadText(label, white);
    }
    virtual ~MenuItem() {}

    void setRect(int x, int y, int w, int h) { rect = {x, y, w, h}; }

    virtual void render() {
        text.render(rect.x + rect.w / 2 - text.getWidth() / 2,
                    rect.y + rect.h / 2 - text.getHeight() / 2);

        SDL_SetRenderDrawColor(Game::gRenderer, 255, 0, 0, 0xFF);
        SDL_RenderDrawRect(Game::gRenderer, &rect);
    }
    // virtual void handleAction() = 0;
    //
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

    void invokeAction() {
        if (action)
            action->execute();
        else
            printf("MenuItem:: action not connected\n");
    };

  protected:
    std::unique_ptr<Action> action;

    SDL_Rect rect;
    TextTexture text;
};

class MenuItemLeaf : public MenuItem {
  public:
    MenuItemLeaf(const std::string &t, int x, int y, int w, int h,
                 std::unique_ptr<Action> a)
        : MenuItem{t, std::move(a)} {
        rect = {x, y, w, h};
    }

    void invokeAction() { action->execute(); }

  private:
};

class MenuBox : public MenuItem {
  public:
    MenuBox(const std::string &label, std::unique_ptr<Action> a)
        : MenuItem(label, std::move(a)) {}

    void addItem(const std::string &name, std::unique_ptr<Action> action) {
        int startX = topleft.x;
        int startY = topleft.y + items.size() * (itemHeight + padding);

        auto item = std::make_unique<MenuItemLeaf>(
            name, startX, startY, itemWidth, itemHeight, std::move(action));
        items.push_back(std::move(item));
    }

    void addItem(std::unique_ptr<MenuItemLeaf> item) {
        // TODO: function calculate start pos for new element
        int startX = topleft.x;
        int startY = topleft.y + items.size() * (itemHeight + padding);

        item->setRect(startY, startY, itemWidth, itemHeight);
        items.push_back(std::move(item));
    }

    void setPadding(int p) { padding = p; }

    virtual void render() override {
        // if (renderAsItem) {
        //     MenuItem::render();
        //     return;
        // }

        for (auto &i : items) {
            i->render();
        }
    }

    bool handleMouse(int mouseX, int mouseY) {
        for (auto &i : items) {
            if (i->isMouseOver(mouseX, mouseY)) {
                i->invokeAction();
                return true;
            }
        }
        return false;
    }

    std::vector<std::unique_ptr<MenuItem>> items;

  private:
    SDL_Point topleft{0, 0};

    int padding = 5;
    int itemWidth = 100;
    int itemHeight = 50;
};

//*******************************************************************************

// TODO: builder pattern
class Menu {
  public:
    void renderMenu() {
        if (!menuStack.empty()) {
            auto &currentMenu = menuStack.top();
            currentMenu->render();
        } else {
            std::cout << "No menu to render!" << std::endl;
        }
    }

    bool handleMouse(int mouseX, int mouseY) {
        if (!menuStack.empty()) {
            for (auto &i : menuStack.top()->items) {
                if (i->isMouseOver(mouseX, mouseY)) {
                    i->invokeAction();
                    return true;
                }
            }
        } else {
            std::cout << "No menu to handleMouse!" << std::endl;
        }
        return false;
    }

    void pushMenu(std::unique_ptr<MenuBox> menuItems) {
        menuStack.push(std::move(menuItems));
    }

    void popMenu() {
        if (!menuStack.empty()) {
            menuStack.pop();
        } else {
            std::cout << "Cannot pop from empty menu stack!" << std::endl;
        }
    }

  private:
    std::stack<std::unique_ptr<MenuBox>> menuStack;
};

#endif // !GAME_MENU_H
