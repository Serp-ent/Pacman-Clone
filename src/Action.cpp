#include "Action.h"

#include "Menu.h"

void OpenSubMenuAction::execute() { menu.pushMenu(std::move(b)); }

void GoBackAction::execute() { menu.popMenu(); }
