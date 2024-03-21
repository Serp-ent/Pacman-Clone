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

class Action;

class MenuItem {
  public:
    MenuItem(const std::string &label, std::unique_ptr<Action> a);
    virtual ~MenuItem() {}

    void setRect(int x, int y, int w, int h);

    virtual void render();
    // virtual void handleAction() = 0;
    //
    bool isMouseOver(int mouseX, int mouseY);

    void invokeAction();

  protected:
    std::unique_ptr<Action> action;

    SDL_Rect rect;
    TextTexture text;
};

class MenuItemLeaf : public MenuItem {
  public:
    MenuItemLeaf(const std::string &t, int x, int y, int w, int h,
                 std::unique_ptr<Action> a);

    void invokeAction();

  private:
};

class MenuBox : public MenuItem {
  public:
    MenuBox(const std::string &label, std::unique_ptr<Action> a);

    void addItem(const std::string &name, std::unique_ptr<Action> action);

    void addItem(std::unique_ptr<MenuItemLeaf> item);

    void setPadding(int p);

    virtual void render() override;
    void render(int x, int y, int w, int h);

    bool handleMouse(int mouseX, int mouseY);

    std::vector<std::unique_ptr<MenuItem>> items;

  private:
    SDL_Point topleft{0, 0}; // FIXME: unused

    int padding = 10;
    int itemWidth = 100; // FIXME: unused
    int itemHeight = 50; // FIXME: unused
};

//*******************************************************************************

// TODO: builder pattern
class Menu {
  public:
    Menu(int x, int y, int w, int h)
        : leftTop{x, y}, elementWidth(w), elementHeight(h) {}

    void renderMenu();

    bool handleMouse(int mouseX, int mouseY);

    void pushMenu(MenuBox &menuItems);

    void popMenu();

  private:
    // don't take ownership
    std::stack<MenuBox *> menuStack;

    SDL_Point leftTop;
    int elementHeight;
    int elementWidth;
};

#endif // !GAME_MENU_H
