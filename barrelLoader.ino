#include "config.h"
#include "menuData.h"
#include "stepperUtils.h"
#include <U8g2lib.h>
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <hardwareUtils.h>
#include <menu.h>
#include <storage.h>

U8G2_SH1107_PIMORONI_128X128_F_HW_I2C u8g2(U8G2_R0);
Storage storage;
StepperUtils sUtils;

void renderPosition();
void renderJog();

class CustomMenu : public Menu {
public:
  void draw(void (*renderFunction)()) override {
    u8g2.firstPage();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 10);
    do {
      renderFunction();
    } while (u8g2.nextPage());
  }

  void drawPartial(void (*renderFunction)()) {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 10);
    renderFunction();
    u8g2.sendBuffer();
  }

  void processItem(const Item *item) override {
    storage.loadFromProgmem(&activeItem, item);
    storage.loadCurrentValueFromEeprom(item, &Item::eepromOffset, currentValue);
    state = State::SETTING;
    if (activePage.name == &jogAxesStr) {
      draw(renderJog);
    } else {
      draw(renderPosition);
    }
  }
};

class CustomHardwareUtils : public HardwareUtils {
public:
  bool detectButton() override {
    if (digitalRead(ENCODER_SW) == LOW) {
      if (millis() - lastButtonPress > 50) {
        lastButtonPress = millis();
        return true;
      }
    }
    return false;
  }

  int8_t detectScroll() override {
    int8_t step = 0;
    currentStateCLK = digitalRead(ENCODER_CLK);
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
      if (digitalRead(ENCODER_DT) != currentStateCLK) {
        step = SCROLL_STEP;
      } else {
        step = -SCROLL_STEP;
      }
    }
    lastStateCLK = currentStateCLK;
    return step;
  }

  void initializePins(const uint8_t *pins, void (*pinModeFunc)(uint8_t)) {
    uint8_t i = 0;
    while (pins[i]) {
      pinModeFunc(pins[i]);
      i++;
    }
  }

  void initEncoder() { lastStateCLK = digitalRead(ENCODER_CLK); }

private:
  int currentStateCLK;
  int lastStateCLK;
  unsigned long lastButtonPress = millis();
};

CustomHardwareUtils utils;
CustomMenu menu;

// --------------------------------------------------------------------------------
// //

void setup() {
  cli();

  Serial.begin(9600);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  utils.initEncoder();

  utils.initializePins(DRV_ENABLE, [](uint8_t pin) {
    pinMode(pin, OUTPUT);
    HardwareUtils::setLow(pin);
  });
  utils.initializePins(ENDSTOP, [](uint8_t pin) {
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin), endstopReached, FALLING);
  });
  utils.initializePins(MESA_OUT, [](uint8_t pin) { pinMode(pin, OUTPUT); });
  utils.initializePins(RELAIS, [](uint8_t pin) { pinMode(pin, OUTPUT); });
  utils.initializePins(MESA_IN, [](uint8_t pin) { pinMode(pin, INPUT); });
  sUtils.initializeSteppers();

  sei();

  u8g2.begin();
  menu.loadPage(&mainMenu);
  menu.navigationHistory[menu.historyIndex] = &mainMenu;
  menu.clear();
}

void loop() {
  switch (menu.state) {
  case State::IDLE:
    handleIdleState();
    break;

  case State::SETTING:
    handleSettingState();
    break;

  default:
    break;
  }
  delay(1);
}

void endstopReached() {
  if (menu.activeItem.name != nullptr) {
    sUtils.stopStepper[menu.activeItem.axis] = true;
  }
}

void freeAxesFunc() {
  memset(sUtils.stopStepper, false, sizeof(sUtils.stopStepper));
  menu.state = State::IDLE;
}

// --------------------------------------------------------------------------------
// //

void handleIdleState() {
  if (utils.detectButton()) {
    menu.handleButtonPressed();
  }
  int8_t dir = utils.detectScroll();
  if (dir != 0) {
    menu.clampAndScroll(dir);
  }
}

void handleSettingState() {
  if (utils.detectButton()) {
    while (!sUtils.destinationReached(menu.activeItem.axis) &&
           !sUtils.stopStepper[menu.activeItem.axis]) {
      sUtils.run(menu.activeItem.axis);
    }
    if (menu.activePage.name != &jogAxesStr) {
      storage.saveToEeprom(&menu.activeItem, &Item::eepromOffset,
                           menu.currentValue);
    }
    menu.clear();
    menu.loadPage(&jogAxes);
    menu.redraw();
  }

  if (!sUtils.stopStepper[menu.activeItem.axis]) {
    int8_t direction = utils.detectScroll();
    if (direction != 0) {
      menu.drawPartial(renderNewPosition);
      menu.currentValue += sUtils.STEP_SIZE * direction;
      sUtils.moveTo(menu.activeItem.axis, menu.currentValue);
    }
    sUtils.run(menu.activeItem.axis);
  }
}

void renderMenu() {
  u8g2.print(storage.printFromProgmem(menu.activePage.name));
  u8g2.setCursor(0, 12);
  u8g2.drawLine(0, 12, 128, 12);

  for (uint8_t i = 0; i < LINES_PER_PAGE; i++) {
    uint8_t itemIndex = menu.topIndex + i;

    uint16_t yPos = 24 + (i * 12);

    if (itemIndex == menu.selectedIndex) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, yPos - 10, 128, 12);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1);
    }

    u8g2.setCursor(2, yPos);
    if (itemIndex <= menu.activePageLength) {
      if (menu.activePage.back != nullptr &&
          itemIndex == menu.activePageLength) {
        u8g2.print(storage.printFromProgmem(menu.activePage.back));
        return;
      }

      MenuComponent component;
      storage.loadFromProgmem(
          &component,
          storage.getFromProgmem(menu.activePage.components, itemIndex));

      u8g2.print(storage.printFromProgmem(component.name));
    }
  }

  u8g2.setDrawColor(1);
}

void renderProgram() { u8g2.print("Running..."); }

void renderNewPosition() {
  u8g2.setCursor(10, 36);
  u8g2.print(menu.currentValue);
}

void renderPosition() {
  u8g2.print(storage.printFromProgmem(menu.activeItem.name));

  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setCursor(5, 24);
  u8g2.print("old: ");
  u8g2.print(menu.currentValue);
  u8g2.setCursor(5, 36);
  u8g2.print("new: ");
  u8g2.print(menu.currentValue);
}

void renderJog() {
  u8g2.print(storage.printFromProgmem(menu.activeItem.name));

  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setCursor(10, 36);
  u8g2.print(menu.currentValue);
}

void renderClearEeprom() { u8g2.print(F("clear eeprom ...")); }

void clearEepromFunc() {
  menu.draw(renderClearEeprom);
  storage.clearEeprom();
  menu.clear();
}

void findHome() {
  menu.draw(renderProgram);
  storage.loadFromProgmem(&menu.activeItem, &X0);
  sUtils.driveToEndstop(0, -1);
  storage.saveToEeprom(&menu.activeItem, &Item::eepromOffset,
                       menu.currentValue);
  delay(sUtils.DELAY_BETWEEN_STEPS);

  storage.loadFromProgmem(&menu.activeItem, &Y0);
  sUtils.driveToEndstop(1, 1);
  storage.saveToEeprom(&menu.activeItem, &Item::eepromOffset,
                       menu.currentValue);
  delay(sUtils.DELAY_BETWEEN_STEPS);

  storage.loadFromProgmem(&menu.activeItem, &Z0);
  sUtils.makeSteps(menu.activeItem.axis, -2500);
  sUtils.driveToEndstop(2, 1);
  storage.saveToEeprom(&menu.activeItem, &Item::eepromOffset,
                       menu.currentValue);
  delay(sUtils.DELAY_BETWEEN_STEPS);
  freeAxesFunc();
  menu.clear();
}

void placeBarrelFunc() {
  Item *pos[] = {&Y0, &Z0};
  sUtils.performStepSequence(pos);

  Item *pos2[] = {&X4};
  sUtils.performStepSequence(pos2);

  Item *pos3[] = {&Y4, &Z4};
  sUtils.performStepSequence(pos3);

  utils.setLow(RELAIS[0]);

  Item *pos4[] = {&Y0, &Z0};
  sUtils.performStepSequence(pos4);

  Item *pos5[] = {&X0};
  sUtils.performStepSequence(pos5);
}

void barrelLoadFunc() {
  Item *pos[] = {&X0, &Y0, &Z0};
  sUtils.performStepSequence(pos);

  utils.setLow(RELAIS[0]);
  utils.setHigh(RELAIS[1]);
  utils.setLow(RELAIS[2]);

  sUtils.makeSteps(3, 400);

  utils.setLow(RELAIS[1]);
  utils.setHigh(RELAIS[2]);

  Item *pos2[] = {&Y1};
  sUtils.performStepSequence(pos2);

  utils.setHigh(RELAIS[0]);
  utils.setLow(RELAIS[2]);

  Item *pos3[] = {&Y0, &Z1};
  sUtils.performStepSequence(pos3);

  Item *pos4[] = {&X0, &Z0};
  sUtils.performStepSequence(pos4);

  sUtils.makeSteps(3, -400);
}

void peakTipFunc() {
  menu.draw(renderProgram);

  Item *X2 = nullptr;
  Item *X3 = nullptr;
  switch (menu.activeProgram.peakMode) {
  case Peak::PEAK_55:
    X2 = &peak55X2;
    X3 = &peak55X3;
    Serial.println("peak55");
    break;

  case Peak::PEAK_60:
    X2 = &peak60X2;
    X3 = &peak60X3;
    break;

  case Peak::SINGLE_SIDE:
    X2 = &singleSideX2;
    break;

  default:
    return;
  }

  utils.setHigh(MESA_IN[0]);

  utils.setLow(MESA_OUT[0]);

  barrelLoadFunc();

  Item *pos[] = {&X1, &Y2, &Z2};
  sUtils.performStepSequence(pos);

  // TODO request open chuck
  Item *pos2[] = {X2};
  sUtils.performStepSequence(pos2);

  // TODO request close chuck

  utils.setLow(RELAIS[0]);

  Item *pos3[] = {&X1, &Y0, &Z0};
  sUtils.performStepSequence(pos3);

  utils.setHigh(MESA_OUT[0]);
  utils.setHigh(MESA_IN[0]);
  utils.setLow(MESA_OUT[0]);

  Item *pos4[] = {X2, &Y2, &Z2};
  sUtils.performStepSequence(pos4);

  utils.setHigh(RELAIS[0]);

  // TODO request open chuck

  Item *pos5[] = {&X1};
  sUtils.performStepSequence(pos5);

  placeBarrelFunc();
  menu.clear();
}
