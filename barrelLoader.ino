#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <EEPROM.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define encoderCLK 49   // Encoder CLK
#define encoderDT 51   // Encoder DT
#define encoderSW 53   // Encoder Taster
#define lengthOfLong sizeof(long)

#define motorInterfaceType AccelStepper::DRIVER
#define X_DRV_DIR 10
#define Y_DRV_DIR 8
#define Z_DRV_DIR 6

#define X_DRV_STEP 11
#define Y_DRV_STEP 9
#define Z_DRV_STEP 7

#define X_DRV_ENABLE 38
#define Y_DRV_ENABLE 56
#define Z_DRV_ENABLE 11

#define X_ENDSTOP 14
#define Y_ENDSTOP 15
#define Z_ENDSTOP 16

#define HOMING_SPEED 2500
#define PROGRAM_SPEED 1000
#define ACCELERATION 750

#define MAX_MENU_ITEMS 5
#define MAX_MENU_POSITIONS 15
#define MAX_MENU_SUBMENUS 5
#define MAX_MENU_PROGRAMS 5
#define MAX_HISTORY 10
#define linesPerPage 8

enum AxisEnum {
  X_AXIS,
  Y_AXIS,
  Z_AXIS
};

struct Axis {
  AxisEnum axis;
  int endstopPin;
};

enum State {
  IDLE,
  SETTING,
  SETPOSITION,
  RUNNING
};

struct Position {
  const char* name;
  Axis* axis;
  long value;
  int eepromOffset;
};

struct Program {
  const char* name;
  void (*programFunction)();
};

struct MenuPage {
    const char* title;
    const char* items[MAX_MENU_ITEMS];
    int menuItemsCount;
    Position* positions[MAX_MENU_POSITIONS];
    int positionsCount;
    MenuPage* subMenus[MAX_MENU_SUBMENUS];
    int subMenusCount;
    Program* programs[MAX_MENU_PROGRAMS];
    int programsCount;

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

State state = IDLE;

Axis X = {X_AXIS, X_ENDSTOP};
Axis Y = {Y_AXIS, Y_ENDSTOP};
Axis Z = {Z_AXIS, Z_ENDSTOP};

//positionen in eprom speichern 
Position X0 = {"X0", &X, 0, 1};
Position Y0 = {"Y0", &Y, 0, 2};
Position Z0 = {"Z0", &Z, 0, 3};

Position X1 = {"X1", &X, 0, 4};
Position Y1 = {"Y1", &Y, 0, 5};
Position Z1 = {"Z1", &Z, 0, 6};

Position peak55X2 = {"X2", &X, 0, 7};
Position peak60X2 = {"X2", &X, 0, 8};
Position singleSideX2 = {"X2", &X, 0, 9};
Position Y2 = {"Y2", &Y, 0, 10};
Position Z2 = {"Z2", &Z, 0, 11};

Position peak55X3 = {"X3", &X, 0, 12};
Position peak60X3 = {"X3", &X, 0, 13};
Position Y3 = {"Y3", &Y, 0, 14};
Position Z3 = {"Z3", &Z, 0, 15};

Position X4 = {"X4", &X, 0, 16};
Position Y4 = {"Y4", &Y, 0, 17};
Position Z4 = {"Z4", &Z, 0, 18};
Position newPosition = {"", &X, 0, -1};

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
Position* selectProgramPositions[] = {nullptr};
MenuPage* selectProgramSubMenus[] = {nullptr};
MenuPage selectProgramMenu("Select Program", selectProgramItems, selectProgramPositions, selectProgramSubMenus);

const char* settingsItems[] = {"Back", nullptr};
Position* settingsPositions[] = {nullptr}; 
MenuPage* settingsSubMenus[] = {&globalPositionsMenu, &singleSideMenu, &peak55Menu, &peak60Menu, nullptr};
MenuPage settingsMenu("Settings", settingsItems, settingsPositions, settingsSubMenus);

const char* mainMenuItems[] = {nullptr}; 
Position* mainMenuPositions[] = {nullptr};
MenuPage* mainMenuSubMenus[] = {&selectProgramMenu, &settingsMenu, nullptr}; 
MenuPage mainMenu("Barrelloader 3.0", mainMenuItems, mainMenuPositions, mainMenuSubMenus);


MenuPage* activePage = &mainMenu;
Position* currentPosition = nullptr;
MenuPage* navigationHistory[MAX_HISTORY];
Program* activeProgram = nullptr;

U8G2_SH1107_PIMORONI_128X128_F_HW_I2C u8g2(U8G2_R0);

int activePageLength() {
  return activePage->menuItemsCount + activePage->subMenusCount + activePage->positionsCount + activePage->programsCount;
}

int historyIndex = -1;
int selectedIndex = 0;
int topIndex = 0;

int eepromAddress = 0;

unsigned long encoderCLKLowTime = 0;
unsigned long encoderDTLowTime = 0;

bool buttonPressed = false;  
bool encoderCLKLowDetected = false;
bool encoderDTLowDetected = false;
long delayMillis = 2;
long lastMillis = millis();

int stepSize = 50; //200 steps = 360°

AccelStepper stepperX(motorInterfaceType, X_DRV_STEP, X_DRV_DIR);
AccelStepper stepperY(motorInterfaceType, Y_DRV_STEP, Y_DRV_DIR);
AccelStepper stepperZ(motorInterfaceType, Z_DRV_STEP, Z_DRV_DIR);
MultiStepper steppersXYZ;
MultiStepper steppersYZ;
MultiStepper steppersXZ;
