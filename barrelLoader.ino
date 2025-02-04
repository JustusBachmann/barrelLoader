#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define lengthOfLong sizeof(long)

const uint8_t ENCODER_CLK = 49;
const uint8_t ENCODER_DT = 51;
const uint8_t ENCODER_SW = 53;

const uint8_t motorInterfaceType = AccelStepper::DRIVER;

const uint8_t DRV_DIR[] = {10, 8, 6, 28, 34}; // X, Y, Z, E0, E1
const uint8_t DRV_STEP[] = {11, 9, 7, 26, 36}; // X, Y, Z, E0, E1
const uint8_t DRV_ENABLE[] = {38, 56, 11, 24, 30}; // X, Y, Z, E0, E1
const uint8_t ENDSTOP[] = {14, 15, 16, 15}; // X, Y, Z, E0                                  ----- E0 = Y? 

const uint8_t MESA_OUT[] = {22, 23, 24};
const uint8_t MESA_IN[] = {29, 30, 31, 32};

const uint8_t RELAIS[] = {26, 27, 28};

const uint16_t HOMING_SPEED = 2500;
const uint16_t PROGRAM_SPEED = 1000;
const uint16_t ACCELERATION = 750;

const uint8_t STEP_SIZE = 50; //200 steps = 360°
const uint16_t DELAY_BETWEEN_STEPS = 1000; //1000 = 1s
const uint8_t DELAY_READ_ENCODER = 2;


const uint8_t MAX_MENU_ITEMS = 1;
const uint8_t MAX_MENU_POSITIONS = 12;
const uint8_t MAX_MENU_SUBMENUS = 5;
const uint8_t MAX_MENU_PROGRAMS = 4;
const uint8_t MAX_HISTORY = 4;
const uint8_t linesPerPage = 8;

enum class Axis: uint8_t {
  X = 0,
  Y = 1,
  Z = 2
};

enum class State: uint8_t {
  IDLE = 0,
  SETPOSITION = 1,
  RUNNING = 2,
  STOP = 3
};

struct Position {
  const char* name;
  Axis axis;
  uint16_t value;
  uint8_t eepromOffset;
};

enum class Peak: uint8_t {
  PEAK_55 = 0,
  PEAK_60 = 1,
  SINGLE_SIDE = 2,
  NONE = 3
};

struct Program {
  const char* name;
  void (*programFunction)();
  Peak peakMode;
};

struct MenuPage {
    const char* title PROGMEM;
    const char* back PROGMEM;
    Position* positions[MAX_MENU_POSITIONS];
    uint8_t positionsCount;
    MenuPage* subMenus[MAX_MENU_SUBMENUS];
    uint8_t subMenusCount;
    Program* programs[MAX_MENU_PROGRAMS];
    uint8_t programsCount;

  MenuPage(const char* t, const char* b, Position* pos[], MenuPage* subs[])
      : title(t), back(b), positionsCount(0), subMenusCount(0) {
      for (positionsCount = 0; positionsCount < MAX_MENU_POSITIONS && pos[positionsCount] != nullptr; positionsCount++) {
          positions[positionsCount] = pos[positionsCount];
      }

      for (subMenusCount = 0; subMenusCount < MAX_MENU_SUBMENUS && subs[subMenusCount] != nullptr; subMenusCount++) {
          subMenus[subMenusCount] = subs[subMenusCount];
      }
  }
};

State state = State::IDLE;
Peak peak = Peak::PEAK_55;

//positionen in eprom speichern 
Position X0 = {"X0", Axis::X, 0, 1};
Position Y0 = {"Y0", Axis::Y, 0, 2};
Position Z0 = {"Z0", Axis::Z, 0, 3};

Position X1 = {"X1", Axis::X, 0, 4};
Position Y1 = {"Y1", Axis::Y, 0, 5};
Position Z1 = {"Z1", Axis::Z, 0, 6};

Position peak55X2 = {"X2", Axis::X, 0, 7};
Position peak60X2 = {"X2", Axis::X, 0, 8};
Position singleSideX2 = {"X2", Axis::X, 0, 9};
Position Y2 = {"Y2", Axis::Y, 0, 10};
Position Z2 = {"Z2", Axis::Z, 0, 11};

Position peak55X3 = {"X3", Axis::X, 0, 12};
Position peak60X3 = {"X3", Axis::X, 0, 13};
Position Y3 = {"Y3", Axis::Y, 0, 14};
Position Z3 = {"Z3", Axis::Z, 0, 15};

Position X4 = {"X4", Axis::X, 0, 16};
Position Y4 = {"Y4", Axis::Y, 0, 17};
Position Z4 = {"Z4", Axis::Z, 0, 18};
Position newPosition = {"", Axis::X, 0, -1};


const char back[] PROGMEM = "Back";
const char peak55[] PROGMEM = "Peak55";
const char peak60[] PROGMEM = "Peak60";
const char singleSide[] PROGMEM = "SingleSide";
const char globalPos[] PROGMEM = "Global Positions";
const char selectProg[] PROGMEM = "Select Program";
const char settings[] PROGMEM = "Settings";
const char barrelLoader[] PROGMEM = "Barrelloader 3.0";

Position* peak55Positions[] = {&peak55X2, &peak55X3, nullptr};
MenuPage* peak55SubMenus[] = {nullptr};
MenuPage peak55Menu(peak55, back, peak55Positions, peak55SubMenus);

Position* peak60Positions[] = {&peak60X2, &peak60X3, nullptr};
MenuPage* peak60SubMenus[] = {nullptr};
MenuPage peak60Menu(peak60, back, peak60Positions, peak60SubMenus);

Position* singleSidePositions[] = {&singleSideX2, nullptr};
MenuPage* singleSideSubMenus[] = {nullptr};
MenuPage singleSideMenu(singleSide, back, singleSidePositions, singleSideSubMenus);

Position* globalPositionsPositions[] = {&X0, &Y0, &Z0, &X1, &Y1, &Z1, &Y2, &Z2, &X4, &Y4, &Z4, nullptr};
MenuPage* globalPositionsSubMenus[] = {nullptr};
MenuPage globalPositionsMenu(globalPos, back, globalPositionsPositions, globalPositionsSubMenus);

Position* selectProgramPositions[] = {nullptr};
MenuPage* selectProgramSubMenus[] = {nullptr};
MenuPage selectProgramMenu(selectProg, back, selectProgramPositions, selectProgramSubMenus);

const char* settingsItems[] = {back, nullptr};
Position* settingsPositions[] = {nullptr}; 
MenuPage* settingsSubMenus[] = {&globalPositionsMenu, &singleSideMenu, &peak55Menu, &peak60Menu, nullptr};
MenuPage settingsMenu(settings, back, settingsPositions, settingsSubMenus);

Position* mainMenuPositions[] = {nullptr};
MenuPage* mainMenuSubMenus[] = {&selectProgramMenu, &settingsMenu, nullptr}; 
MenuPage mainMenu(barrelLoader, nullptr, mainMenuPositions, mainMenuSubMenus);


MenuPage* activePage = &mainMenu;
Position* currentPosition = nullptr;
MenuPage* navigationHistory[MAX_HISTORY];
Program* activeProgram = nullptr;

struct EncoderState {
    unsigned long clkLowTime;
    unsigned long dtLowTime;
    bool buttonPressed;
    bool clkLowDetected;
    bool dtLowDetected;
};
EncoderState encoderState = {0, 0, false, false, false};
unsigned long lastMillis = millis();

uint8_t activePageLength() {
  return (activePage->back == nullptr ? 0 : 1) + activePage->subMenusCount + activePage->positionsCount + activePage->programsCount;
}

uint8_t historyIndex = 0;
uint8_t selectedIndex = 0;
uint8_t topIndex = 0;

U8G2_SH1107_PIMORONI_128X128_F_HW_I2C u8g2(U8G2_R0);

AccelStepper steppers[5]; // X, Y, Z, E0, E1


MultiStepper steppersXYZ;
MultiStepper steppersYZ;
MultiStepper steppersXZ;
