#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <AccelStepper.h>
#include <EEPROM.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define motorInterfaceType 1 // 1 = Stepper, 2 = AccelStepper
#define dirPinX 10
#define stepPinX 11

#define encoderCLK 49
#define encoderDT 51
#define encoderSW 53

#define STEP_SIZE 1
#define linesPerPage 8
#define MAX_MENU_ITEMS 5
#define MAX_MENU_POSITIONS 15
#define MAX_MENU_SUBMENUS 5
#define MAX_HISTORY 10

AccelStepper stepperX(motorInterfaceType, stepPinX, dirPinX);

int eeAddress = 0;
int eeOffset = sizeof(float);

U8G2_SH1107_PIMORONI_128X128_F_HW_I2C u8g2(U8G2_R0);


bool buttonPressed = false;  
bool encoderCLKLowDetected = false;
bool encoderDTLowDetected = false;

unsigned long encoderCLKLowTime = 0;
unsigned long encoderDTLowTime = 0;

float newPosition = 0.0f;
bool newPositionSet = false;
enum State {MENU, POSITIONING, SETTING};
State state = MENU;

struct Position {
  const char* name;
  char axis;
  float value;
  int eeAddress;
};

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

Position X0 = {"X0", "X", 0.0f, eeAddress + eeOffset};
Position Y0 = {"Y0", "Y", 0.0f, eeAddress + (2 * eeOffset)};
Position Z0 = {"Z0", "Z", 0.0f, eeAddress + (3 * eeOffset)};

Position X1 = {"X1", "X", 0.0f, eeAddress + (4 * eeOffset)};
Position Y1 = {"Y1", "Y", 0.0f, eeAddress + (5 * eeOffset)};
Position Z1 = {"Z1", "Z", 0.0f, eeAddress + (6 * eeOffset)};

Position singleSideX2 = {"X2", "X", 0.0f, eeAddress + (7 * eeOffset)};
Position peak55X2 = {"X2", "X", 0.0f, eeAddress + (8 * eeOffset)};
Position peak60X2 = {"X2", "X", 0.0f, eeAddress + (9 * eeOffset)};
Position Y2 = {"Y2", "Y", 0.0f, eeAddress + (10 * eeOffset)};
Position Z2 = {"Z2", "Z", 0.0f, eeAddress + (11 * eeOffset)};

Position peak55X3 = {"X3", "X", 0.0f, eeAddress + (12 * eeOffset)};
Position peak60X3 = {"X3", "X", 0.0f, eeAddress + (13 * eeOffset)};
Position Y3 = {"Y3", "Y", 0.0f, eeAddress + (14 * eeOffset)};
Position Z3 = {"Z3", "Z", 0.0f, eeAddress + (15 * eeOffset)};

Position X4 = {"X4", "X", 0.0f, eeAddress + (16 * eeOffset)};
Position Y4 = {"Y4", "Y", 0.0f, eeAddress + (17 * eeOffset)};
Position Z4 = {"Z4", "Z", 0.0f, eeAddress + (18 * eeOffset)};

Position* homePositions[] = {&X0, &Y0, &Z0, nullptr};

const char* peak55Items[] = {"Back", nullptr};
Position* peak55Positions[] = {&peak55X2, &peak55X3, nullptr};
MenuPage* peak55SubMenus[] = {nullptr};
MenuPage peak55Menu("Peak55", peak55Items, peak55Positions, peak55SubMenus);

const char* peak60Items[] = {"Back", nullptr};
Position* peak60Positions[] = {&peak60X2, &peak60X3, nullptr};
MenuPage* peak60SubMenus[] = {nullptr};
MenuPage peak60Menu("Peak60", peak60Items, peak60Positions, peak60SubMenus);

const char* singleSideItems[] = {"Back", nullptr};
Position* singleSidePositions[] = {&singleSideX2, nullptr};
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

void setup() {
  Serial.begin(9600);

  pinMode(encoderCLK, INPUT_PULLUP);
  pinMode(encoderDT,  INPUT_PULLUP);
  pinMode(encoderSW,  INPUT_PULLUP);

  stepperX.setMaxSpeed(1000);       // Maximum speed in steps per second
  stepperX.setAcceleration(500);   // Acceleration in steps per second^2

  u8g2.begin();

  draw(renderLoadingScreen);
  loadAllPositions(homePositions, 3);
  updatePosition(&X0, 0.0f);
  updatePosition(&Y0, 0.0f);
  updatePosition(&Z0, 0.0f);
  updatePosition(&singleSideX2, 0.0f);
  loadAllPositions(activePage->positions, activePage->positionsCount);
  
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
      switch (state) {
        case MENU:
          clampAndScroll(+1);
          draw(renderMenu);
          break;
        case POSITIONING:
          newPosition -= 1;
          break;
        case SETTING:
          positionSetting(+1);
          break;
      }
    } else if (encoderDTLowTime < encoderCLKLowTime) {
      switch (state) {
        case MENU:
          clampAndScroll(-1);
          draw(renderMenu);
          break;
        case POSITIONING:
          newPosition -= 1;
          break;
        case SETTING:
          positionSetting(-1);          
          break;
      }
    }

    resetDetection();
  }
}

void resetDetection() {
  encoderCLKLowDetected = false;
  encoderDTLowDetected = false;
  encoderCLKLowTime = 0;
  encoderDTLowTime = 0;
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

void positionSetting(int direction) {
  switch (activePage->positions[selectedIndex - activePage->subMenusCount]->axis) {
    case 'X':
      if (newPosition + direction * STEP_SIZE >= X0.value) {
        newPosition += direction * STEP_SIZE;
        stepperX.moveTo(direction * STEP_SIZE);
      } else {
        newPosition = 0;
      } 
      break;
    case 'Y':
      if (newPosition + direction * STEP_SIZE >= Y0.value) {
        newPosition += direction * STEP_SIZE;
      } else {
        newPosition = 0;
      }
      break;
    case 'Z':
      if (newPosition + direction * STEP_SIZE >= Z0.value) {
        newPosition += direction * STEP_SIZE;
      } else {
        newPosition = 0;
      }
      break;
  }
  draw(renderPosition);
}

void renderPosition() {
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  u8g2.print(activePage->positions[selectedIndex - activePage->subMenusCount]->name);

  u8g2.setCursor(0, 12);
  u8g2.print("___________________________");

  u8g2.setCursor(5, 24);
  if (newPositionSet) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, 24 - 10, 128, 12);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1); 
    }
  u8g2.print(newPosition);
  Serial.print("New position: ");
  Serial.println(newPosition);
}


void handleButtonPressed() {
  if (state == MENU) {
    if (selectedIndex < activePage->subMenusCount) {
      changePage(activePage->subMenus[selectedIndex]);
      draw(renderMenu);
      return;
    }
    int selectedPositionItem = selectedIndex - activePage->subMenusCount;
    if (selectedPositionItem < activePage->positionsCount) {
      state = SETTING;
      draw(renderPosition);
      return;
    }

    int selectedMenuItem = selectedIndex - activePage->subMenusCount - activePage->positionsCount;
    if (strcmp(activePage->items[selectedMenuItem], "Back") == 0) {
      goBack();
      draw(renderMenu);
      return;
    }
  } else if (state == SETTING) {
    if (newPositionSet) {
      newPositionSet = false;
      state = MENU;
      activePage->positions[selectedIndex - activePage->subMenusCount]->value = newPosition;
      updatePosition(activePage->positions[selectedIndex - activePage->subMenusCount], newPosition);
      draw(renderMenu);
    } else {
      newPositionSet = true;
      newPosition = activePage->positions[selectedIndex - activePage->subMenusCount]->value;
      draw(renderPosition);
    }
  }
}
