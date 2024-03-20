#include "Action.h"

#include "Menu.h"

void OpenSubMenuAction::execute() { menu.pushMenu(*b); }

void GoBackAction::execute() { menu.popMenu(); }
