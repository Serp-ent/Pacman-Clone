#include "Menu.h"
#include "Action.h"

MenuItem::MenuItem(const std::string &label, std::unique_ptr<Action> a)
    : action{std::move(a)} {
    text.loadText(label, white);
}
void MenuItem::setRect(int x, int y, int w, int h) { rect = {x, y, w, h}; }

void MenuItem::render() {
    text.render(rect.x + rect.w / 2 - text.getWidth() / 2,
                rect.y + rect.h / 2 - text.getHeight() / 2);

    SDL_SetRenderDrawColor(Game::gRenderer, 255, 0, 0, 0xFF);
    SDL_RenderDrawRect(Game::gRenderer, &rect);
}
// virtual void handleAction() = 0;
//
bool MenuItem::isMouseOver(int mouseX, int mouseY) {
    // TODO: use a SDL_inRect function or something like that
    if (rect.x > mouseX || rect.x + rect.w < mouseX) {
        return false;
    }
    if (rect.y > mouseY || rect.y + rect.h < mouseY) {
        return false;
    }

    return true;
}

void MenuItem::invokeAction() {
    if (action)
        action->execute();
    else
        printf("MenuItem:: action not connected\n");
};

MenuItemLeaf::MenuItemLeaf(const std::string &t, int x, int y, int w, int h,
                           std::unique_ptr<Action> a)
    : MenuItem{t, std::move(a)} {
    rect = {x, y, w, h};
}

void MenuItemLeaf::invokeAction() { action->execute(); }

MenuBox::MenuBox(const std::string &label, std::unique_ptr<Action> a)
    : MenuItem(label, std::move(a)) {}

void MenuBox::addItem(const std::string &name, std::unique_ptr<Action> action) {
    int startX = topleft.x;
    int startY = topleft.y + items.size() * (itemHeight + padding);

    auto item = std::make_unique<MenuItemLeaf>(name, startX, startY, itemWidth,
                                               itemHeight, std::move(action));
    items.push_back(std::move(item));
}

void MenuBox::addItem(std::unique_ptr<MenuItemLeaf> item) {
    // TODO: function calculate start pos for new element
    int startX = topleft.x;
    int startY = topleft.y + items.size() * (itemHeight + padding);

    item->setRect(startY, startY, itemWidth, itemHeight);
    items.push_back(std::move(item));
}

void MenuBox::setPadding(int p) { padding = p; }

void MenuBox::render() {
    // if (renderAsItem) {
    //     MenuItem::render();
    //     return;
    // }

    for (auto &i : items) {
        i->render();
    }
}

bool MenuBox::handleMouse(int mouseX, int mouseY) {
    for (auto &i : items) {
        if (i->isMouseOver(mouseX, mouseY)) {
            i->invokeAction();
            return true;
        }
    }
    return false;
}

// TODO: builder pattern
void Menu::renderMenu() {
    if (!menuStack.empty()) {
        auto &currentMenu = menuStack.top();
        currentMenu->render();
    } else {
        std::cout << "No menu to render!" << std::endl;
    }
}

bool Menu::handleMouse(int mouseX, int mouseY) {
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

void Menu::pushMenu(MenuBox &menuItems) { menuStack.push(&menuItems); }

void Menu::popMenu() {
    if (!menuStack.empty()) {
        menuStack.pop();
    } else {
        std::cout << "Cannot pop from empty menu stack!" << std::endl;
    }
}
