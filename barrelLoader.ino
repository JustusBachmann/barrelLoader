#include <Wire.h>
// #include <Arduino.h>
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
    const char* name PROGMEM;
    Axis axis;
    uint8_t eepromOffset;
};

struct DynPosition {
    Position* pos;
    uint16_t value;
};

enum class Peak : uint8_t {
    PEAK_55 = 0,
    PEAK_60 = 1,
    SINGLE_SIDE = 2,
    NONE = 3
};

struct Program {
    const char* name PROGMEM; 
    void (*programFunction)();
    Peak peakMode;
};

struct MenuPage {
    const char* title PROGMEM;
    const char* back PROGMEM;                 
    const MenuPage* const* subMenus PROGMEM;
    uint8_t subMenusCount;
    const Position* const* positions PROGMEM; 
    uint8_t positionsCount;
    const Program* const* programs PROGMEM;
    uint8_t programsCount;
};

State state = State::IDLE;
Peak peak = Peak::PEAK_55;

const Position X0 PROGMEM = {"X0", Axis::X, 1};
const Position Y0 PROGMEM = {"Y0", Axis::Y, 2};
const Position Z0 PROGMEM = {"Z0", Axis::Z, 3};

const Position X1 PROGMEM = {"X1", Axis::X, 4};
const Position Y1 PROGMEM = {"Y1", Axis::Y, 5};
const Position Z1 PROGMEM = {"Z1", Axis::Z, 6};

const Position peak55X2 PROGMEM = {"X2", Axis::X, 7};
const Position peak60X2 PROGMEM = {"X2", Axis::X, 8};
const Position singleSideX2 PROGMEM = {"X2", Axis::X, 9};
const Position Y2 PROGMEM = {"Y2", Axis::Y, 10};
const Position Z2 PROGMEM = {"Z2", Axis::Z, 11};

const Position peak55X3 PROGMEM = {"X3", Axis::X, 12};
const Position peak60X3 PROGMEM = {"X3", Axis::X, 13};
const Position Y3 PROGMEM = {"Y3", Axis::Y, 14};
const Position Z3 PROGMEM = {"Z3", Axis::Z, 15};

const Position X4 PROGMEM = {"X4", Axis::X, 16};
const Position Y4 PROGMEM = {"Y4", Axis::Y, 17};
const Position Z4 PROGMEM = {"Z4", Axis::Z, 18};

DynPosition currentPosition = {nullptr, 0};
DynPosition newPosition = {nullptr, 0};

void findHome();
void placeBarrelFunc();
void barrelLoadFunc();
void peakTipFunc();
void clearEEPROM();
void loadActivePage(MenuPage* menuPtr);
void changePage(MenuPage* newActive);
void printSubMenuFromProgmem(MenuPage* subMenuPtr);
void printProgramFromProgmem(Program* programPtr);
void printPositionFromProgmem(Position* positionPtr);
void loadMenuPageFromProgmem(MenuPage* page, MenuPage* menuPtr);
const MenuPage* getSubMenuFromProgmem(const MenuPage* const* subMenusArray, uint8_t index);
const Program* getProgramFromProgmem(const Program* const* programsArray, uint8_t index);
const Position* getPositionFromProgmem(const Position* const* positionsArray, uint8_t index);

const char homingStr[] PROGMEM = "Homing";
const char placeBarrelStr[] PROGMEM = "Place Barrel";
const char loadBarrelStr[] PROGMEM = "Load Barrel";
const char peak55TipStr[] PROGMEM = "Peak 55 Tip";
const char peak60TipStr[] PROGMEM = "Peak 60 Tip";
const char peakSiSiTipStr[] PROGMEM = "Peak Single Side";
const char clearEepromStr[] PROGMEM = "Clear EEPROM";

const Program homing PROGMEM = {homingStr, &findHome, Peak::NONE};
const Program placeBarrel PROGMEM = {placeBarrelStr, &placeBarrelFunc, Peak::NONE};
const Program loadBarrel PROGMEM = {loadBarrelStr, &barrelLoadFunc, Peak::NONE};
const Program peak55Tip PROGMEM = {peak55TipStr, &peakTipFunc, Peak::PEAK_55};
const Program peak60Tip PROGMEM = {peak60TipStr, &peakTipFunc, Peak::PEAK_60};
const Program peakSiSiTip PROGMEM = {peakSiSiTipStr, &peakTipFunc, Peak::SINGLE_SIDE};  
const Program clearEeprom PROGMEM = {clearEepromStr, &clearEEPROM, Peak::NONE};

// Define Strings stored in PROGMEM
const char back[] PROGMEM = "Back";
const char peak55[] PROGMEM = "Peak55";
const char peak60[] PROGMEM = "Peak60";
const char singleSide[] PROGMEM = "SingleSide";
const char globalPos[] PROGMEM = "Global Positions";
const char selectProg[] PROGMEM = "Select Program";
const char settings[] PROGMEM = "Settings";
const char barrelLoader[] PROGMEM = "Barrelloader 3.0";

// Define Arrays of Positions stored in PROGMEM
const Position* const peak55Positions[] PROGMEM = {&peak55X2, &peak55X3};
const Position* const peak60Positions[] PROGMEM = {&peak60X2, &peak60X3};
const Position* const singleSidePositions[] PROGMEM = {&singleSideX2};
const Position* const globalPositionsPositions[] PROGMEM =
    {&X0, &Y0, &Z0, &X1, &Y1, &Z1, &Y2, &Z2, &X4, &Y4, &Z4};
const Position* const settingsPositions[] PROGMEM = {nullptr};
const Position* const selectProgramPositions[] PROGMEM = {nullptr};
const Position* const mainMenuPositions[] PROGMEM = {nullptr};


const MenuPage* const peak55SubMenus[] PROGMEM = {nullptr};
const MenuPage* const peak60SubMenus[] PROGMEM = {nullptr};
const MenuPage* const singleSideSubMenus[] PROGMEM = {nullptr};
const MenuPage* const globalPositionsSubMenus[] PROGMEM = {nullptr};

const MenuPage* const peak55Programs[] PROGMEM = {nullptr};
const MenuPage* const peak60Programs[] PROGMEM = {nullptr};
const MenuPage* const peakSiSiPrograms[] PROGMEM = {nullptr};
const MenuPage* const globalPositionsPrograms[] PROGMEM = {nullptr};

// Define MenuPages stored in PROGMEM
const MenuPage peak55Menu PROGMEM = {
    peak55,
    back,
    (MenuPage**)peak55SubMenus,
    0,
    (Position**)peak55Positions,
    2,
    (Program**)peak55Programs,
    0};

const MenuPage peak60Menu PROGMEM = {
    peak60,
    back,
    (MenuPage**)peak60SubMenus,
    0,
    (Position**)peak60Positions,
    2,
    (Program**)peak60Programs,
    0};

const MenuPage singleSideMenu PROGMEM = {
    singleSide,
    back,
    (MenuPage**)singleSideSubMenus,
    0,
    (Position**)singleSidePositions,
    1,
    (Program**)peakSiSiPrograms,
    0};

const MenuPage globalPositionsMenu PROGMEM = {
    globalPos,
    back,
    (MenuPage**)globalPositionsSubMenus,
    0,
    (Position**)globalPositionsPositions,
    11,
    (Program**)globalPositionsPrograms,
    0};

// Define Main Menu Submenus stored in PROGMEM
const MenuPage* const settingsSubMenus[] PROGMEM =
    {&globalPositionsMenu, &singleSideMenu, &peak55Menu, &peak60Menu};

const Program* const settingsPrograms[] PROGMEM =
    {&clearEeprom};


const MenuPage settingsMenu PROGMEM = {
    settings,
    back,
    (MenuPage**)settingsSubMenus,
    4,
    (Position**)settingsPositions,
    0,
    (Program**)settingsPrograms,
    1};

const MenuPage* const selectProgramSubMenus[] PROGMEM =
    {nullptr};

const Program* const selectProgramPrograms[] PROGMEM =
    {&placeBarrel, &loadBarrel, &peak55Tip, &peak60Tip, &peakSiSiTip};

const MenuPage selectProgramMenu PROGMEM = {
    selectProg,
    back,
    (MenuPage**)selectProgramSubMenus,
    0,
    (Position**)selectProgramPositions,
    0,
    (Program**)selectProgramPrograms,
    5};

const MenuPage* const mainMenuSubMenus[] PROGMEM =
    {&selectProgramMenu, &settingsMenu};

const Program* const mainMenuPrograms[] PROGMEM =
    {&homing};

const MenuPage mainMenu PROGMEM = {
    barrelLoader,
    nullptr,
    (MenuPage**)mainMenuSubMenus,
    2,
    (Position**)mainMenuPositions,
    0,
    (Program**)mainMenuPrograms,
    1};


MenuPage activePage;
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

uint8_t activePageLength = 0; 

uint8_t historyIndex = 0;
uint8_t selectedIndex = 0;
uint8_t topIndex = 0;

U8G2_SH1107_PIMORONI_128X128_F_HW_I2C u8g2(U8G2_R0);

AccelStepper steppers[5]; // X, Y, Z, E0, E1

MultiStepper steppersXYZ;
MultiStepper steppersYZ;
MultiStepper steppersXZ;
