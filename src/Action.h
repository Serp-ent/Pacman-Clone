#ifndef GAME_ACTION_H
#define GAME_ACTION_H

#include "Menu.h"
#include <memory>
#include <stdexcept>
#include <stdio.h>

class Menu;

class Action {
  public:
    virtual ~Action() {}

    virtual void execute() = 0;
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

class GoBackAction : public Action {
  public:
    GoBackAction(Menu &m) : menu{m} {}

    void execute() override;

  private:
    Menu &menu;
};

class OpenSubMenuAction : public Action {
  public:
    OpenSubMenuAction(Menu &m, std::unique_ptr<MenuBox> box)
        : menu{m}, b{std::move(box)} {
        if (!b) {
            throw std::runtime_error(
                "OpenSubMenuAction: MenuBox argument is null");
        }
    }

    void execute() override;

  private:
    // arguments for execute
    Menu &menu;
    std::unique_ptr<MenuBox> b;
};

#endif // !GAME_ACTION_H
