#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SH1107_PIMORONI_128X128_F_HW_I2C u8g2(U8G2_R0);

const int encoderCLK = 49;   // Encoder CLK
const int encoderDT  = 51;   // Encoder DT
const int encoderSW  = 53;   // Encoder Taster

bool buttonPressed = false;  
bool encoderCLKLowDetected = false;
bool encoderDTLowDetected = false;

unsigned long encoderCLKLowTime = 0;
unsigned long encoderDTLowTime = 0;


struct Position {
  const char* name;
  int value;
};

Position X0 = {"X0", -1};
Position Y0 = {"Y0", -1};
Position Z0 = {"Z0", -1};

Position X1 = {"X1", -1};
Position Y1 = {"Y1", -1};
Position Z1 = {"Z1", -1};

Position X2 = {"X2", -1};
Position Y2 = {"Y2", -1};
Position Z2 = {"Z2", -1};

Position X3 = {"X3", -1};
Position Y3 = {"Y3", -1};
Position Z3 = {"Z3", -1};

Position X4 = {"X4", -1};
Position Y4 = {"Y4", -1};
Position Z4 = {"Z4", -1};

const int MAX_MENU_ITEMS = 5;
const int MAX_MENU_POSITIONS = 15;
const int MAX_MENU_SUBMENUS = 5;
const int MAX_HISTORY = 10; // Maximum depth of menu hierarchy

struct MenuPage {
    const char* title;
    const char* items[MAX_MENU_ITEMS];
    int menuItemsCount;
    Position* positions[MAX_MENU_POSITIONS];
    int positionsCount;
    MenuPage* subMenus[MAX_MENU_SUBMENUS];
    int subMenusCount;

    MenuPage(const char* t, const char* itms[], Position* pos[], MenuPage* subs[])
        : title(t), menuItemsCount(0), positionsCount(0), subMenusCount(0) {
        for (int i = 0; i < MAX_MENU_ITEMS && itms[i] != nullptr; i++) {
            items[i] = itms[i];
            menuItemsCount++;
        }

        for (int i = 0; i < MAX_MENU_POSITIONS && pos[i] != nullptr; i++) {
            positions[i] = pos[i];
            positionsCount++;
        }

        for (int i = 0; i < MAX_MENU_SUBMENUS && subs[i] != nullptr; i++) {
            subMenus[i] = subs[i];
            subMenusCount++;
        }
    }
};

const char* peak55Items[] = {"Back", nullptr};
Position* peak55Positions[] = {&X2, &X3, nullptr};
MenuPage* peak55SubMenus[] = {nullptr};
MenuPage peak55Menu("Peak55", peak55Items, peak55Positions, peak55SubMenus);

const char* peak60Items[] = {"Back", nullptr};
Position* peak60Positions[] = {&X2, &X3, nullptr};
MenuPage* peak60SubMenus[] = {nullptr};
MenuPage peak60Menu("Peak60", peak60Items, peak60Positions, peak60SubMenus);

const char* singleSideItems[] = {"Back", nullptr};
Position* singleSidePositions[] = {&X2, nullptr};
MenuPage* singleSideSubMenus[] = {nullptr};
MenuPage singleSideMenu("SingleSide", singleSideItems, singleSidePositions, singleSideSubMenus);

const char* globalPositionsItems[] = {"Back", nullptr};
Position* globalPositionsPositions[] = {&X0, &Y0, &Z0, &X1, &Y1, &Z1, &Y2, &Z2, &X4, &Y4, &Z4, nullptr};
MenuPage* globalPositionsSubMenus[] = {nullptr};
MenuPage globalPositionsMenu("Global Positions", globalPositionsItems, globalPositionsPositions, globalPositionsSubMenus);

const char* selectProgramItems[] = {"Back", nullptr};
Position* selectProgramPositions[] = {nullptr}; // No positions for this menu
MenuPage* selectProgramSubMenus[] = {&singleSideMenu, &peak55Menu, &peak60Menu, nullptr};
MenuPage selectProgramMenu("Select Program", selectProgramItems, selectProgramPositions, selectProgramSubMenus);

const char* settingsItems[] = {"Back", nullptr};
Position* settingsPositions[] = {nullptr}; // No positions for this menu
MenuPage* settingsSubMenus[] = {&globalPositionsMenu, &singleSideMenu, &peak55Menu, &peak60Menu, nullptr};
MenuPage settingsMenu("Settings", settingsItems, settingsPositions, settingsSubMenus);

const char* mainMenuItems[] = {"Homing", nullptr}; 
Position* mainMenuPositions[] = {nullptr}; // No positions for this menu
MenuPage* mainMenuSubMenus[] = {&selectProgramMenu, &settingsMenu, nullptr}; 
MenuPage mainMenu("Barrelloader 3.0", mainMenuItems, mainMenuPositions, mainMenuSubMenus);


MenuPage* activePage = &mainMenu;
int activePageLength() {
  return activePage->menuItemsCount + activePage->subMenusCount + activePage->positionsCount;
}
int historyIndex = -1;
MenuPage* navigationHistory[MAX_HISTORY];


int selectedIndex = 0;
int topIndex = 0;
const int linesPerPage = 8;


void setup() {
  Serial.begin(115200);

  // Pins für Encoder
  pinMode(encoderCLK, INPUT_PULLUP);
  pinMode(encoderDT,  INPUT_PULLUP);
  pinMode(encoderSW,  INPUT_PULLUP);

  u8g2.begin();
  draw(renderMenu);
}

void loop() {
  if (digitalRead(encoderSW) == LOW) {
    waitForButtonRelease();
    handleButtonPressed();
  }

  if (digitalRead(encoderCLK) == LOW && !encoderCLKLowDetected) {
    encoderCLKLowDetected = true;
    encoderCLKLowTime = millis(); 
  }

  if (digitalRead(encoderDT) == LOW && !encoderDTLowDetected) {
    encoderDTLowDetected = true;
    encoderDTLowTime = millis();
  }

  if (encoderCLKLowDetected && encoderDTLowDetected) {
    if (encoderCLKLowTime < encoderDTLowTime) {
      clampAndScroll(+1);
    } else if (encoderDTLowTime < encoderCLKLowTime) {
      clampAndScroll(-1);
    }

    resetDetection();
    draw(renderMenu);
  }
}

void resetDetection() {
  encoderCLKLowDetected = false;
  encoderDTLowDetected = false;
  encoderCLKLowTime = 0;
  encoderDTLowTime = 0;
}

void changePage(MenuPage* newActive) {
  if (historyIndex < MAX_HISTORY - 1) {
      navigationHistory[++historyIndex] = activePage; 
  }
  activePage = newActive;
  selectedIndex = 0;
  topIndex = 0;
}

void goBack() {
  if (historyIndex >= 0) {
    activePage = navigationHistory[historyIndex--];
    selectedIndex = 0;
    topIndex = 0; 
  }
}

void waitForButtonRelease() {
  const unsigned long debounceDelay = 50;
  unsigned long lastDebounceTime = 0;

  while (true) {
    bool currentState = digitalRead(encoderSW);
    if (currentState == HIGH) {
      if ((millis() - lastDebounceTime) > debounceDelay) {
        break;
      }
    } else {
      lastDebounceTime = millis();
    }
  }

  buttonPressed = false;
}

void clampAndScroll(int direction) {
  if (selectedIndex + direction >= 0 && selectedIndex + direction < activePageLength()) {
    selectedIndex += direction;
  }

  if (selectedIndex < topIndex) {
    topIndex = selectedIndex;
  } else if (selectedIndex > topIndex + (linesPerPage)) {
    topIndex = selectedIndex - (linesPerPage);
  }
}

void draw(void (*renderFunction)()) {
  u8g2.firstPage();
  do {
    renderFunction();
  } while (u8g2.nextPage());
}

void renderMenu() {
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  u8g2.print(activePage->title);

  u8g2.setCursor(0, 12);
  u8g2.print("___________________________");

  for (int i = 0; i <= linesPerPage; i++) {
    int itemIndex = topIndex + i;
    if (itemIndex >= activePageLength()) break;

    int yPos = 24 + (i * 12); 

    if (itemIndex == selectedIndex) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, yPos - 10, 128, 12);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1); 
    }

    u8g2.setCursor(2, yPos);
    if (itemIndex < activePage->subMenusCount) {
      u8g2.print(activePage->subMenus[itemIndex]->title);
    } else if (itemIndex - activePage->subMenusCount < activePage->positionsCount) {
      u8g2.print(activePage->positions[itemIndex - activePage->subMenusCount]->name);
    } else {
      u8g2.print(activePage->items[itemIndex - activePage->subMenusCount - activePage->positionsCount]);
    }
  }

  u8g2.setDrawColor(1);
}

void renderPosition() {
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  u8g2.print(activePage->positions[selectedIndex - activePage->subMenusCount]->name);

  u8g2.setCursor(0, 12);
  u8g2.print("___________________________");

  u8g2.setCursor(5, 24);
  u8g2.print(activePage->positions[selectedIndex - activePage->subMenusCount]->value);
}



void handleButtonPressed() {
    if (selectedIndex < activePage->subMenusCount) {
      changePage(activePage->subMenus[selectedIndex]);
      draw(renderMenu);
      return;
    }
    int selectedPositionItem = selectedIndex - activePage->subMenusCount;
    if (selectedPositionItem < activePage->positionsCount) {
      draw(renderPosition);
      return;
    }

    int selectedMenuItem = selectedIndex - activePage->subMenusCount - activePage->positionsCount;
    if (strcmp(activePage->items[selectedMenuItem], "Back") == 0) {
      goBack();
      draw(renderMenu);
      return;
    }
    

//----------------------------------dont change Logic above----------------------------------//
    // Handle other actions here
    Serial.print("Selected action: ");
    Serial.println(activePage->items[selectedMenuItem]);
}
