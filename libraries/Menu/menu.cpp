#include <menu.h>
#include <storage.h>

extern Storage storage;
extern MenuPage mainMenu;
void renderMenu();

void Menu::handleButtonPressed() {
  if (selectedIndex > activePage.componentCount) {
    goBack();
    return;
  }
  MenuComponent component;
  storage.loadFromProgmem(&component, storage.getFromProgmem(activePage.components, selectedIndex));

  switch (component.type) {
    case MenuComponent::PAGE:
      navigateToSubMenu(component.page);
      break;

    case MenuComponent::PROGRAM:
      processProgram(component.program);
      break;

    case MenuComponent::ITEM:
      processItem(component.item);
      break;
  }
}

void Menu::clampAndScroll(int8_t direction) {
  if (selectedIndex + direction >= 0 && selectedIndex + direction <= activePageLength) {
    selectedIndex += direction;

    if (selectedIndex < topIndex) {
      topIndex = selectedIndex; // Scroll up.
    } else if (selectedIndex >= topIndex + LINES_PER_PAGE) {
      topIndex = selectedIndex - (LINES_PER_PAGE - 1); // Scroll down.
    }

    redraw();
  }
}

void Menu::processProgram(const Program* program) {
  storage.loadFromProgmem(&activeProgram, program);
  state = State::WORKING;

  activeProgram.programFunction();
}

void Menu::processItem(const Item* item) {
  storage.loadFromProgmem(&activeItem, item);
  state = State::SETTING;
  redraw();
}

void Menu::navigateToSubMenu(const MenuPage* subMenu) {
  if (historyIndex < MAX_HISTORY - 1) {
    navigationHistory[++historyIndex] = subMenu;
  }
  loadPage(subMenu);
  clear();
}

void Menu::goBack() {
  if (historyIndex > 0) {
    loadPage(navigationHistory[--historyIndex]);
  } else if (historyIndex == 0) {
    loadPage(navigationHistory[historyIndex]);
  }
  clear();
}

void Menu::loadPage(const MenuPage* menuPtr) {
  storage.loadFromProgmem(&activePage, menuPtr);
  activePageLength = activePage.componentCount + (activePage.back != nullptr ? 1 : 0 );
}

void Menu::redraw() {
  draw(renderMenu);
}

void Menu::clear() {
  state = State::IDLE;
  selectedIndex = 0;
  topIndex = 0;
  currentValue = 0;
  activeItem = {};
  activeProgram = {};
  redraw();
}