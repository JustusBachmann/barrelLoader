#ifndef MENUDATA_H
#define MENUDATA_H

#include "config.h"
#include "menu.h"
#include "storage.h"
#include <avr/pgmspace.h>

const char xStr[] PROGMEM = "X";
const char yStr[] PROGMEM = "Y";
const char zStr[] PROGMEM = "Z";
const char x0Str[] PROGMEM = "X0";
const char y0Str[] PROGMEM = "Y0";
const char z0Str[] PROGMEM = "Z0";
const char x1Str[] PROGMEM = "X1";
const char y1Str[] PROGMEM = "Y1";
const char z1Str[] PROGMEM = "Z1";
const char x2Str[] PROGMEM = "X2";
const char y2Str[] PROGMEM = "Y2";
const char z2Str[] PROGMEM = "Z2";
const char x3Str[] PROGMEM = "X3";
const char y3Str[] PROGMEM = "Y3";
const char z3Str[] PROGMEM = "Z3";
const char x4Str[] PROGMEM = "X4";
const char y4Str[] PROGMEM = "Y4";
const char z4Str[] PROGMEM = "Z4";
const char backStr[] PROGMEM = "Back";
const char peak55[] PROGMEM = "Peak55";
const char peak60[] PROGMEM = "Peak60";
const char singleSide[] PROGMEM = "SingleSide";
const char globalPos[] PROGMEM = "Global Positions";
const char selectProg[] PROGMEM = "Select Program";
const char jogAxesStr[] PROGMEM = "jog axes";
const char settingsStr[] PROGMEM = "Settings";
const char barrelLoader[] PROGMEM = "Barrelloader 3.0";
const char homingStr[] PROGMEM = "Homing";
const char freeAxisStr[] PROGMEM = "unlock axes";
const char placeBarrelStr[] PROGMEM = "Place Barrel";
const char loadBarrelStr[] PROGMEM = "Load Barrel";
const char peak55TipStr[] PROGMEM = "Peak 55 Tip";
const char peak60TipStr[] PROGMEM = "Peak 60 Tip";
const char peakSiSiTipStr[] PROGMEM = "Peak Single Side";
const char clearEepromStr[] PROGMEM = "Clear EEPROM";

const Item X PROGMEM = {xStr, 0, 255};
const Item Y PROGMEM = {y0Str, 1, 255};
const Item Z PROGMEM = {z0Str, 2, 255};

const Item X0 PROGMEM = {x0Str, 0, 1};
const Item Y0 PROGMEM = {y0Str, 1, 2};
const Item Z0 PROGMEM = {z0Str, 2, 3};

const Item X1 PROGMEM = {x1Str, 0, 4};
const Item Y1 PROGMEM = {y1Str, 1, 5};
const Item Z1 PROGMEM = {z1Str, 2, 6};

const Item peak60X2 PROGMEM = {x2Str, 0, 8};
const Item peak55X2 PROGMEM = {x2Str, 0, 7};
const Item singleSideX2 PROGMEM = {x2Str, 0, 9};
const Item Y2 PROGMEM = {y2Str, 1, 10};
const Item Z2 PROGMEM = {z2Str, 2, 11};

const Item peak55X3 PROGMEM = {x3Str, 0, 12};
const Item peak60X3 PROGMEM = {x3Str, 0, 13};
const Item Y3 PROGMEM = {y3Str, 1, 14};
const Item Z3 PROGMEM = {z3Str, 2, 15};

const Item X4 PROGMEM = {x4Str, 0, 16};
const Item Y4 PROGMEM = {y4Str, 1, 17};
const Item Z4 PROGMEM = {z4Str, 2, 18};

void findHome();
const Program homing PROGMEM = {homingStr, &findHome, Peak::NONE};
void freeAxesFunc();
const Program freeAxis PROGMEM = {freeAxisStr, &freeAxesFunc, Peak::NONE};
void placeBarrelFunc();
const Program placeBarrel PROGMEM = {placeBarrelStr, &placeBarrelFunc, Peak::NONE};
void barrelLoadFunc();
const Program loadBarrel PROGMEM = {loadBarrelStr, &barrelLoadFunc, Peak::NONE};
void peakTipFunc();
const Program peak55TipProg PROGMEM = {peak55TipStr, &peakTipFunc, Peak::PEAK_55};
const Program peak60TipProg PROGMEM = {peak60TipStr, &peakTipFunc, Peak::PEAK_60};
const Program peakSiSiTipProg PROGMEM = {peakSiSiTipStr, &peakTipFunc, Peak::SINGLE_SIDE};  
void clearEepromFunc();
const Program clearEepromProg PROGMEM = {clearEepromStr, &clearEepromFunc, Peak::NONE};

const MenuComponent peak55Tip PROGMEM = { MenuComponent::PROGRAM, { .program = &peak55TipProg }, peak55TipStr };
const MenuComponent peak60Tip PROGMEM = { MenuComponent::PROGRAM, { .program = &peak60TipProg }, peak60TipStr };
const MenuComponent peakSiSiTip PROGMEM = { MenuComponent::PROGRAM, { .program = &peakSiSiTipProg }, peakSiSiTipStr };
const MenuComponent *const selectProgramComponents[] PROGMEM = { &peak55Tip, &peak60Tip, &peakSiSiTip };
const MenuPage selectProgram PROGMEM = { selectProg, backStr, selectProgramComponents, 2 };

const MenuComponent compItemX0 PROGMEM = { MenuComponent::ITEM, { .item = &X0 }, x0Str };
const MenuComponent compItemY0 PROGMEM = { MenuComponent::ITEM, { .item = &Y0 }, y0Str };
const MenuComponent compItemZ0 PROGMEM = { MenuComponent::ITEM, { .item = &Z0 }, z0Str };

const MenuComponent compItemX1 PROGMEM = { MenuComponent::ITEM, { .item = &X1 }, x1Str };
const MenuComponent compItemY1 PROGMEM = { MenuComponent::ITEM, { .item = &Y1 }, y1Str };
const MenuComponent compItemZ1 PROGMEM = { MenuComponent::ITEM, { .item = &Z1 }, z1Str };

const MenuComponent compItemPeak55X2 PROGMEM = { MenuComponent::ITEM, { .item = &peak55X2 }, x2Str };
const MenuComponent compItemPeak60X2 PROGMEM = { MenuComponent::ITEM, { .item = &peak60X2 }, x2Str };
const MenuComponent compItemPeakSiSiX2 PROGMEM = { MenuComponent::ITEM, { .item = &singleSideX2 }, x2Str };
const MenuComponent compItemY2 PROGMEM = { MenuComponent::ITEM, { .item = &Y2 }, y2Str };
const MenuComponent compItemZ2 PROGMEM = { MenuComponent::ITEM, { .item = &Z2 }, z2Str };

const MenuComponent compItemPeak55X3 PROGMEM = { MenuComponent::ITEM, { .item = &peak55X3 }, x3Str };
const MenuComponent compItemPeak60X3 PROGMEM = { MenuComponent::ITEM, { .item = &peak60X3 }, x3Str };
const MenuComponent compItemY3 PROGMEM = { MenuComponent::ITEM, { .item = &Y3 }, y3Str };
const MenuComponent compItemZ3 PROGMEM = { MenuComponent::ITEM, { .item = &Z3 }, z3Str };

const MenuComponent compItemX4 PROGMEM = { MenuComponent::ITEM, { .item = &X4 }, x4Str };
const MenuComponent compItemY4 PROGMEM = { MenuComponent::ITEM, { .item = &Y4 }, y4Str };
const MenuComponent compItemZ4 PROGMEM = { MenuComponent::ITEM, { .item = &Z4 }, z4Str };


const MenuComponent *const peak55Positions[] PROGMEM = {&compItemPeak55X2, &compItemPeak55X3};
const MenuComponent *const peak60Positions[] PROGMEM = {&compItemPeak60X2, &compItemPeak60X3};
const MenuComponent *const singleSidePositions[] PROGMEM = {&compItemPeakSiSiX2};
const MenuComponent *const globalPositionsPositions[] PROGMEM =
    {/*&compItemX0, &compItemY0, &compItemZ0, */&compItemX1, &compItemY1, &compItemZ1, &compItemY2, &compItemZ2, &compItemX4, &compItemY4, &compItemZ4};

const MenuPage peak55Menu PROGMEM = { peak55, backStr, peak55Positions, 1 };
const MenuPage peak60Menu PROGMEM = { peak60, backStr, peak60Positions, 1 };
const MenuPage peakSiSiMenu PROGMEM = { singleSide, backStr, singleSidePositions, 0 };
const MenuPage globalPositionsMenu PROGMEM = { globalPos, backStr, globalPositionsPositions, 7 };

const MenuComponent compItemX PROGMEM = { MenuComponent::ITEM, { .item = &X }, xStr };
const MenuComponent compItemY PROGMEM = { MenuComponent::ITEM, { .item = &Y }, yStr };
const MenuComponent compItemZ PROGMEM = { MenuComponent::ITEM, { .item = &Z }, zStr };
const MenuComponent *const jogAxesComps[] PROGMEM =
    {&compItemX, &compItemY, &compItemZ};
const MenuPage jogAxes PROGMEM = { jogAxesStr, backStr, jogAxesComps, 2 };

const MenuComponent compClearEepromProg PROGMEM = { MenuComponent::PROGRAM, { .program = &clearEepromProg }, clearEepromStr };
const MenuComponent compFreeAxisProg PROGMEM = { MenuComponent::PROGRAM, { .program = &freeAxis }, freeAxisStr };
const MenuComponent compPeak55 PROGMEM = { MenuComponent::PAGE, { .page = &peak55Menu }, peak55 };
const MenuComponent compPeak60 PROGMEM = { MenuComponent::PAGE, { .page = &peak60Menu }, peak60 };
const MenuComponent compPeakSiSi PROGMEM = { MenuComponent::PAGE, { .page = &peakSiSiMenu }, singleSide };
const MenuComponent compGlobalPositions PROGMEM = { MenuComponent::PAGE, { .page = &globalPositionsMenu }, globalPos };
const MenuComponent *const settingsComponents[] PROGMEM = {&compFreeAxisProg, &compGlobalPositions, &compPeakSiSi, &compPeak55, &compPeak60, &compClearEepromProg };
const MenuPage settingsMenu PROGMEM = { barrelLoader, backStr, settingsComponents, 5 };

const MenuComponent compSelectProgram PROGMEM = { MenuComponent::PAGE, { .page = &selectProgram }, selectProg };
const MenuComponent compJogAxes PROGMEM = { MenuComponent::PAGE, { .page = &jogAxes }, jogAxesStr };
const MenuComponent compSettings PROGMEM = { MenuComponent::PAGE, { .page = &settingsMenu }, settingsStr };
const MenuComponent compHoming PROGMEM = { MenuComponent::PROGRAM, { .program = &homing }, homingStr };
const MenuComponent *const mainMenuComponents[] PROGMEM = { &compSelectProgram, &compJogAxes, &compSettings, &compHoming };
const MenuPage mainMenu PROGMEM = { barrelLoader, nullptr, mainMenuComponents, 3 };

#endif