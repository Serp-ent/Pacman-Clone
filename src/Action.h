#ifndef GAME_ACTION_H
#define GAME_ACTION_H

#include "Menu.h"
#include <cstring>
#include <dirent.h>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <vector>

class Menu;

class Action {
  public:
    virtual ~Action() {}

    virtual void execute() = 0;
};

class ListHighScores : public Action {
  public:
    ListHighScores();
    virtual void execute() override {
        for (int i = 0; i < exampleData.size(); ++i) {
            printf("[%d] %s\n", i, exampleData[i].c_str());
        }
    }

  private:
    std::vector<std::string> exampleData = {"marek: 123", "andrzej: 255",
                                            "Agata: 200"};
};

class ListLevelsInGrid : public Action {
  public:
    void execute() override {
        std::unique_ptr<DIR, DirDeleter> dir{opendir(dirLevel.c_str())};
        if (!dir) {
            printf("Can't open directory %s\n", dirLevel.c_str());
            return;
        }

        for (dirent *f = readdir(dir.get()); f != nullptr;
             f = readdir(dir.get())) {
            if (strcmp("..", f->d_name) == 0 || strcmp(".", f->d_name) == 0) {
                continue;
            }

            printf("%s\n", f->d_name);
        }
    }

  private:
    struct DirDeleter {
        void operator()(DIR *dir) const {
            if (dir) {
                closedir(dir);
            }
        }
    };

    const std::string dirLevel = "./levels";
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

    void execute() override { gameStarted = !gameStarted; }

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
