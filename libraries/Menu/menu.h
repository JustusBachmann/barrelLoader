#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

#define MAX_HISTORY 4
#define LINES_PER_PAGE 8


enum class State : uint8_t { WAITING = 0, IDLE = 1, WORKING = 2, SETTING = 3 };

enum class Peak : uint8_t
{
    PEAK_55 = 0,
    PEAK_60 = 1,
    SINGLE_SIDE = 2,
    NONE = 3
};

struct Item {
  const char *name;
  uint8_t axis;
  uint8_t eepromOffset;
};

struct Program {
  const char *name;
  void (*programFunction)();
  Peak peakMode;
};

struct MenuPage;

struct MenuComponent {
  enum Type { PAGE, PROGRAM, ITEM } type;
  union {
    const MenuPage *page;
    const Program *program;
    const Item *item;
  };
  const char *name;
};

struct MenuPage {
  const char *name;
  const char *back;
  const MenuComponent *const *components;
  uint8_t componentCount;
};

class Menu {
  public:
    MenuPage activePage;
    Program activeProgram;
    Item activeItem;
    uint16_t currentValue = 0;
    MenuPage *navigationHistory[MAX_HISTORY];
    uint8_t historyIndex = 0;
    volatile State state = State::IDLE;
    int8_t selectedIndex = 0;
    uint8_t activePageLength = 0;
    uint8_t topIndex = 0;

    void handleButtonPressed();
    virtual void draw(void (*renderFunction)());
    void clampAndScroll(int8_t direction);
    void navigateToSubMenu(const MenuPage *newActive);
    virtual void processProgram(const Program *program);
    virtual void processItem(const Item *item);
    void goBack();
    void loadPage(const MenuPage* menuPtr);
    void clear();
    void redraw();
};

#endif // MENU_H
