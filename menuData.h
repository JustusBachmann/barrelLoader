#ifndef MENUDATA_H
#define MENUDATA_H

#include "config.h"
#include "menu.h"
#include "storage.h"

const char xStr[] = "X";
const char yStr[] = "Y";
const char zStr[] = "Z";
const char x0Str[] = "X0";
const char y0Str[] = "Y0";
const char z0Str[] = "Z0";
const char x1Str[] = "X1";
const char y1Str[] = "Y1";
const char z1Str[] = "Z1";
const char x2Str[] = "X2";
const char y2Str[] = "Y2";
const char z2Str[] = "Z2";
const char x3Str[] = "X3";
const char y3Str[] = "Y3";
const char z3Str[] = "Z3";
const char x4Str[] = "X4";
const char y4Str[] = "Y4";
const char z4Str[] = "Z4";
const char backStr[] = "Back";
const char peak55[] = "Peak55";
const char peak60[] = "Peak60";
const char singleSide[] = "SingleSide";
const char globalPos[] = "Global Positions";
const char selectProg[] = "Select Program";
const char jogAxesStr[] = "jog axes";
const char settingsStr[] = "Settings";
const char barrelLoader[] = "Barrelloader 3.0";
const char homingStr[] = "Homing";
const char freeAxisStr[] = "unlock axes";
const char placeBarrelStr[] = "Place Barrel";
const char loadBarrelStr[] = "Load Barrel";
const char peak55TipStr[] = "Peak 55 Tip";
const char peak60TipStr[] = "Peak 60 Tip";
const char peakSiSiTipStr[] = "Peak Single Side";
const char clearEepromStr[] = "Clear EEPROM";

const Item X = {xStr, 0, 255};
const Item Y = {y0Str, 1, 255};
const Item Z = {z0Str, 2, 255};

const Item X0 = {x0Str, 0, 1};
const Item Y0 = {y0Str, 1, 2};
const Item Z0 = {z0Str, 2, 3};

const Item X1 = {x1Str, 0, 4};
const Item Y1 = {y1Str, 1, 5};
const Item Z1 = {z1Str, 2, 6};

const Item peak60X2 = {x2Str, 0, 8};
const Item peak55X2 = {x2Str, 0, 7};
const Item singleSideX2 = {x2Str, 0, 9};
const Item Y2 = {y2Str, 1, 10};
const Item Z2 = {z2Str, 2, 11};

const Item peak55X3 = {x3Str, 0, 12};
const Item peak60X3 = {x3Str, 0, 13};
const Item Y3 = {y3Str, 1, 14};
const Item Z3 = {z3Str, 2, 15};

const Item X4 = {x4Str, 0, 16};
const Item Y4 = {y4Str, 1, 17};
const Item Z4 = {z4Str, 2, 18};

void findHome();
const Program homing = {homingStr, &findHome, Peak::NONE};
void freeAxesFunc();
const Program freeAxis = {freeAxisStr, &freeAxesFunc, Peak::NONE};
void placeBarrelFunc();
const Program placeBarrel = {placeBarrelStr, &placeBarrelFunc, Peak::NONE};
void barrelLoadFunc();
const Program loadBarrel = {loadBarrelStr, &barrelLoadFunc, Peak::NONE};
void peakTipFunc();
const Program peak55TipProg = {peak55TipStr, &peakTipFunc, Peak::PEAK_55};
const Program peak60TipProg = {peak60TipStr, &peakTipFunc, Peak::PEAK_60};
const Program peakSiSiTipProg = {peakSiSiTipStr, &peakTipFunc, Peak::SINGLE_SIDE};  
void clearEepromFunc();
const Program clearEepromProg = {clearEepromStr, &clearEepromFunc, Peak::NONE};

const MenuComponent peak55Tip = { MenuComponent::PROGRAM, { .program = &peak55TipProg }, peak55TipStr };
const MenuComponent peak60Tip = { MenuComponent::PROGRAM, { .program = &peak60TipProg }, peak60TipStr };
const MenuComponent peakSiSiTip = { MenuComponent::PROGRAM, { .program = &peakSiSiTipProg }, peakSiSiTipStr };
const MenuComponent *const selectProgramComponents[] = { &peak55Tip, &peak60Tip, &peakSiSiTip };
const MenuPage selectProgram = { selectProg, backStr, selectProgramComponents, 2 };

const MenuComponent compItemX0 = { MenuComponent::ITEM, { .item = &X0 }, x0Str };
const MenuComponent compItemY0 = { MenuComponent::ITEM, { .item = &Y0 }, y0Str };
const MenuComponent compItemZ0 = { MenuComponent::ITEM, { .item = &Z0 }, z0Str };

const MenuComponent compItemX1 = { MenuComponent::ITEM, { .item = &X1 }, x1Str };
const MenuComponent compItemY1 = { MenuComponent::ITEM, { .item = &Y1 }, y1Str };
const MenuComponent compItemZ1 = { MenuComponent::ITEM, { .item = &Z1 }, z1Str };

const MenuComponent compItemPeak55X2 = { MenuComponent::ITEM, { .item = &peak55X2 }, x2Str };
const MenuComponent compItemPeak60X2 = { MenuComponent::ITEM, { .item = &peak60X2 }, x2Str };
const MenuComponent compItemPeakSiSiX2 = { MenuComponent::ITEM, { .item = &singleSideX2 }, x2Str };
const MenuComponent compItemY2 = { MenuComponent::ITEM, { .item = &Y2 }, y2Str };
const MenuComponent compItemZ2 = { MenuComponent::ITEM, { .item = &Z2 }, z2Str };

const MenuComponent compItemPeak55X3 = { MenuComponent::ITEM, { .item = &peak55X3 }, x3Str };
const MenuComponent compItemPeak60X3 = { MenuComponent::ITEM, { .item = &peak60X3 }, x3Str };
const MenuComponent compItemY3 = { MenuComponent::ITEM, { .item = &Y3 }, y3Str };
const MenuComponent compItemZ3 = { MenuComponent::ITEM, { .item = &Z3 }, z3Str };

const MenuComponent compItemX4 = { MenuComponent::ITEM, { .item = &X4 }, x4Str };
const MenuComponent compItemY4 = { MenuComponent::ITEM, { .item = &Y4 }, y4Str };
const MenuComponent compItemZ4 = { MenuComponent::ITEM, { .item = &Z4 }, z4Str };


const MenuComponent *const peak55Positions[] = {&compItemPeak55X2, &compItemPeak55X3};
const MenuComponent *const peak60Positions[] = {&compItemPeak60X2, &compItemPeak60X3};
const MenuComponent *const singleSidePositions[] = {&compItemPeakSiSiX2};
const MenuComponent *const globalPositionsPositions[] =
    {/*&compItemX0, &compItemY0, &compItemZ0, */&compItemX1, &compItemY1, &compItemZ1, &compItemY2, &compItemZ2, &compItemX4, &compItemY4, &compItemZ4};

const MenuPage peak55Menu = { peak55, backStr, peak55Positions, 1 };
const MenuPage peak60Menu = { peak60, backStr, peak60Positions, 1 };
const MenuPage peakSiSiMenu = { singleSide, backStr, singleSidePositions, 0 };
const MenuPage globalPositionsMenu = { globalPos, backStr, globalPositionsPositions, 7 };

const MenuComponent compItemX = { MenuComponent::ITEM, { .item = &X }, xStr };
const MenuComponent compItemY = { MenuComponent::ITEM, { .item = &Y }, yStr };
const MenuComponent compItemZ = { MenuComponent::ITEM, { .item = &Z }, zStr };
const MenuComponent *const jogAxesComps[] =
    {&compItemX, &compItemY, &compItemZ};
const MenuPage jogAxes = { jogAxesStr, backStr, jogAxesComps, 2 };

const MenuComponent compClearEepromProg = { MenuComponent::PROGRAM, { .program = &clearEepromProg }, clearEepromStr };
const MenuComponent compFreeAxisProg = { MenuComponent::PROGRAM, { .program = &freeAxis }, freeAxisStr };
const MenuComponent compPeak55 = { MenuComponent::PAGE, { .page = &peak55Menu }, peak55 };
const MenuComponent compPeak60 = { MenuComponent::PAGE, { .page = &peak60Menu }, peak60 };
const MenuComponent compPeakSiSi = { MenuComponent::PAGE, { .page = &peakSiSiMenu }, singleSide };
const MenuComponent compGlobalPositions = { MenuComponent::PAGE, { .page = &globalPositionsMenu }, globalPos };
const MenuComponent *const settingsComponents[] = {&compFreeAxisProg, &compGlobalPositions, &compPeakSiSi, &compPeak55, &compPeak60, &compClearEepromProg };
const MenuPage settingsMenu = { barrelLoader, backStr, settingsComponents, 5 };

const MenuComponent compSelectProgram = { MenuComponent::PAGE, { .page = &selectProgram }, selectProg };
const MenuComponent compJogAxes = { MenuComponent::PAGE, { .page = &jogAxes }, jogAxesStr };
const MenuComponent compSettings = { MenuComponent::PAGE, { .page = &settingsMenu }, settingsStr };
const MenuComponent compHoming = { MenuComponent::PROGRAM, { .program = &homing }, homingStr };
const MenuComponent *const mainMenuComponents[] = { &compSelectProgram, &compJogAxes, &compSettings, &compHoming };
const MenuPage mainMenu = { barrelLoader, nullptr, mainMenuComponents, 3 };

#endif