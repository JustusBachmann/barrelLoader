#include "config.h"
#include "menuData.h"
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <hardwareUtils.h>
#include <menu.h>
#include <storage.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SH1107_PIMORONI_128X128_F_HW_I2C u8g2(U8G2_R0);
AccelStepper steppers[5]; // X, Y, Z, E0, E1
MultiStepper steppersXYZ;
Storage storage;

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

  void processProgram(const Program *program) override {
    TIMSK1 &= ~(1 << OCIE2A); // Disable timer interrupt
    // void processProgramItem(int programIndex) {
    //     const Program *programPtr = getFromProgmem(activePage.programs,
    //     programIndex); if (checkCrc() || programPtr == &clearEepromProg) {
    //       state = State::RUNNING;
    //       Program activeProgram;
    //       loadFromProgmem(&activeProgram, programPtr);
    //       draw(renderProgram);
    //       printFromProgmem(activeProgram.name);
    //       peak = activeProgram.peakMode;
    //       activeProgram.programFunction();
    //       state = State::IDLE;
    //     }
    //     draw(renderMenu);
    //   }
    TIMSK1 |= (1 << OCIE2A); // Enable timer interrupt
  }

  void processItem(const Item *item) override {
    // void processPositionItem(int positionIndex) {
    //     if (checkCrc()) {
    //       const Position *positionPtr =
    //           getFromProgmem(activePage.positions, positionIndex);
    //       loadCurrentPosFromEeprom(positionPtr);
    //       draw(renderPosition);
    //       state = State::SETPOSITION;
    //       DynPosition *positions[1];
    //       positions[0] = {&currentPosition};
    //       step(positions, 1);
    //     } else {
    //       draw(renderMenu);
    //     }
    //   }
  }
};

class CustomHardwareUtils : public HardwareUtils {
public:
  struct EncoderState {
    unsigned long clkLowTime;
    unsigned long dtLowTime;
    bool buttonPressed;
    bool clkLowDetected;
    bool dtLowDetected;
  };

  EncoderState encoderState = {0, 0, false, false, false};
  unsigned long lastMillis = millis();

  bool detectButton() override {
    if (digitalRead(ENCODER_SW) == LOW) {
      waitForButtonRelease();
      return true;
    }
    return false;
  }

  int8_t detectScroll() override {
    if (millis() - lastMillis >= DELAY_READ_ENCODER) {
      lastMillis = millis();
      if (digitalRead(ENCODER_CLK) == LOW && !encoderState.clkLowDetected) {
        encoderState.clkLowDetected = true;
        encoderState.clkLowTime = millis();
      }
  
      if (digitalRead(ENCODER_DT) == LOW && !encoderState.dtLowDetected) {
        encoderState.dtLowDetected = true;
        encoderState.dtLowTime = millis();
      }
    }

    if (encoderState.clkLowDetected && encoderState.dtLowDetected) {
      if (encoderState.clkLowTime < encoderState.dtLowTime) {
        return SCROLL_STEP;
      } else if (encoderState.dtLowTime < encoderState.clkLowTime) {
        return -SCROLL_STEP;
      }
      resetDetection();
    }
    return 0;
  }

private:
  void resetDetection() { encoderState = {0, 0, false, false, false}; }

  void waitForButtonRelease() {
    const unsigned long debounceDelay = 50;
    unsigned long lastDebounceTime = 0;

    while (true) {
      bool currentState = digitalRead(ENCODER_SW);
      if (currentState == HIGH) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
          break;
        }
      } else {
        lastDebounceTime = millis();
      }
    }
    encoderState.buttonPressed = false;
  }
};

CustomHardwareUtils utils;
CustomMenu menu;

const uint8_t motorInterfaceType = AccelStepper::DRIVER;

struct DynPosition {
  Item *pos;
  uint8_t axis; // X = 0, Y = 1, Z = 2
  int16_t value;
};

DynPosition currentPosition = {nullptr, -1, -1};
DynPosition newPosition = {nullptr, -1, -1};
volatile bool reloadTriggered = false;

ISR(TIMER1_COMPA_vect) {
  if (menu.state == State::SETTING) {
    reloadTriggered = true; // Set flag to lock screen
  }
}

void setup() {
  cli();
  // Timer1 Configuration
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 62499;           // Set compare match value for a 5-second delay
  TCCR1B |= (1 << WGM12);  // Configure Timer1 in CTC mode
  TCCR1B |= (1 << CS12);   // Set prescaler to 256
  TIMSK1 |= (1 << OCIE1A); // Enable Timer Compare Interrupt

  Serial.begin(115200);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  for (uint8_t i = 0; i < 5; i++) {
    pinMode(DRV_ENABLE[i], OUTPUT);
    HardwareUtils::setLow(DRV_ENABLE[i]);
  }

  for (uint8_t i = 0; i < 4; i++) {
    pinMode(ENDSTOP[i], INPUT_PULLUP);
  }

  for (uint8_t i = 0; i < 3; i++) {
    pinMode(MESA_OUT[i], OUTPUT);
  }

  for (uint8_t i = 0; i < 3; i++) {
    pinMode(RELAIS[i], OUTPUT);
  }

  for (uint8_t i = 0; i < 3; i++) {
    pinMode(MESA_IN[i], INPUT);
  }

  u8g2.begin();
  menu.loadPage(&mainMenu);
  menu.navigationHistory[menu.historyIndex] = &mainMenu;
  menu.clear();

  sei();
}

void loop() {
  delay(1);

  switch (menu.state) {
  case State::IDLE:
    if (utils.detectButton()) {
      menu.handleButtonPressed();
    }

    int8_t dir = utils.detectScroll();
    if (dir != 0) {
      menu.clampAndScroll(dir);
    }
    return;

  case State::SETTING:
    if (reloadTriggered) {
      reloadTriggered = false; // Reset flag
      menu.draw(renderPosition);
    }

    if (utils.detectButton()) {
      while (!destinationReached(currentPosition.axis)) {
        steppers[currentPosition.axis].run();
      }
      saveNewPosition();
    }

    int8_t direction = utils.detectScroll();
    if (direction != 0) {
      setPosition(direction);
    }

    if (stop(currentPosition.axis)) {
      steppers[currentPosition.axis].stop();
      return;
    }
    steppers[currentPosition.axis].run();
    return;

  default:
    return;
  }
}

void setPosition(int8_t dir) {
  newPosition.value += STEP_SIZE * dir;
  DynPosition *positions[1];
  positions[0] = {&newPosition};
  step(positions, 1);
}

void saveNewPosition() {
  if (currentPosition.pos != nullptr) {
    currentPosition.value = newPosition.value;
    // saveToEeprom(&currentPosition);
  }
  menu.clear();
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

void renderPosition() {
  //   printFromProgmem(currentPosition.pos);

  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setCursor(5, 24);
  u8g2.print("old: ");
  u8g2.print(currentPosition.value);
  u8g2.setCursor(5, 36);
  u8g2.print("new: ");
  u8g2.print(newPosition.value);
}

// void driveToEndstop(uint8_t axis, int8_t dir) {
//     steppers[axis].setSpeed(dir * HOMING_SPEED);
//     while (!stop(axis)) {
//       steppers[axis].runSpeed();
//     }
//     steppers[axis].stop();
//     steppers[axis].setCurrentPosition(0);
//   }

  bool stop(uint8_t axis) {
    return digitalRead(ENDSTOP[axis]) == LOW;
  }

//   void makeSteps(uint8_t axis, int steps, int8_t dir) {
//     steppers[axis].move(dir * steps);
//     steppers[axis].runToPosition();
//   }

void step(DynPosition *pos[], uint8_t count) {
  //     long destPos[3] = {steppers[0].currentPosition(),
  //     steppers[1].currentPosition(), steppers[2].currentPosition()}; for
  //     (uint8_t i = 0; i < count; i++) {
  //       if (pos[i] != nullptr) {
  //         destPos[pos[i]->axis] = pos[i]->value;
  //       }
  //     }
  //     steppersXYZ.moveTo(destPos);
}

//   void performStepSequence(Item* xPos, Item* yPos, Item* zPos, DynPosition*
//   positions[]) {
//     if (xPos) getFromEeprom(xPos, positions[0]);
//     if (yPos) getFromEeprom(yPos, positions[1]);
//     if (zPos) getFromEeprom(zPos, positions[2]);
//     step(positions, 3);
//     steppersXYZ.runSpeedToPosition();
//     delay(DELAY_BETWEEN_STEPS);
//   }

bool destinationReached(int stepperID) {
  if (steppers[stepperID].distanceToGo() == 0) {
    return true;
  }
  return false;
}

void initializeSteppers() {
  for (int i = 0; i < 5; i++) {
    steppers[i] = AccelStepper(motorInterfaceType, DRV_STEP[i], DRV_DIR[i]);
    steppers[i].setMaxSpeed(PROGRAM_SPEED);
    steppers[i].setAcceleration(ACCELERATION);
  }

  steppersXYZ.addStepper(steppers[0]);
  steppersXYZ.addStepper(steppers[1]);
  steppersXYZ.addStepper(steppers[2]);
}

void findHome() {
  // loadCurrentPosFromEeprom(&X0);
  // driveToEndstop(0, -1);
  // currentPosition.value = 0;
  // saveToEeprom(&currentPosition);

  // loadCurrentPosFromEeprom(&Y0);
  // driveToEndstop(1, 1);
  // currentPosition.value = 0;
  // saveToEeprom(&currentPosition);

  // makeSteps(2, 2500, -1);
  // loadCurrentPosFromEeprom(&Z0);
  // driveToEndstop(2, 1);
  // currentPosition.value = 0;
  // saveToEeprom(&currentPosition);
}

void placeBarrelFunc(DynPosition *xDynPos, DynPosition *yDynPos,
                     DynPosition *zDynPos) {
  // DynPosition* positions[3] = {xDynPos, yDynPos, zDynPos};

  // performStepSequence(nullptr, &Y0, &Z0, positions);

  // performStepSequence(&X4, nullptr, nullptr, positions);

  // performStepSequence(nullptr, &Y4, &Z4, positions);

  // setLow(RELAIS[0]);

  // performStepSequence(nullptr, &Y0, &Z0, positions);

  // performStepSequence(&X0, nullptr, nullptr, positions);
}

void barrelLoadFunc(DynPosition *xDynPos, DynPosition *yDynPos,
                    DynPosition *zDynPos) {
  // DynPosition* positions[3] = {xDynPos, yDynPos, zDynPos};

  // performStepSequence(&X0, &Y0, &Z0, positions);

  // setLow(RELAIS[0]);
  // setHigh(RELAIS[1]);
  // setLow(RELAIS[2]);

  // makeSteps(3, 400, 1);

  // delay(1000);

  // setLow(RELAIS[1]);
  // setHigh(RELAIS[2]);

  // performStepSequence(nullptr, &Y1, nullptr, positions);

  // setHigh(RELAIS[0]);
  // setLow(RELAIS[2]);

  // performStepSequence(nullptr, &Y0, &Z1, positions);

  // performStepSequence(&X0, nullptr, &Z0, positions);

  // makeSteps(3, 400, -1);
}

void peakTipFunc() {
  // DynPosition xDynPos, yDynPos, zDynPos;
  // DynPosition* positions[3] = {&xDynPos, &yDynPos, &zDynPos};

  // Position* X2 = nullptr;
  // Position* X3 = nullptr;

  // switch(peak) {
  //   case Peak::PEAK_55:
  //     X2 = &peak55X2;
  //     X3 = &peak55X3;
  //     break;

  //   case Peak::PEAK_60:
  //     X2 = &peak60X2;
  //     X3 = &peak60X3;
  //     break;

  //   case Peak::SINGLE_SIDE:
  //     X2 = &singleSideX2;
  //     break;

  //   default:
  //     return;
  // }

  // setHigh(MESA_IN[0]);

  // setLow(MESA_OUT[0]);

  // barrelLoadFunc(&xDynPos, &yDynPos, &zDynPos);

  // performStepSequence(&X1, &Y2, &Z2, positions);

  // // TODO request open chuck

  // performStepSequence(X2, nullptr, nullptr, positions);

  // // TODO request close chuck

  // setLow(RELAIS[0]);

  // performStepSequence(&X1, &Y0, &Z0, positions);

  // setHigh(MESA_OUT[0]);
  // setHigh(MESA_IN[0]);
  // setLow(MESA_OUT[0]);

  // performStepSequence(X2, &Y2, &Z2, positions);

  // setHigh(RELAIS[0]);

  // // TODO request open chuck

  // performStepSequence(&X1, nullptr, nullptr, positions);

  // placeBarrelFunc(&xDynPos, &yDynPos, &zDynPos);
}

void renderClearEeprom() { u8g2.print(F("clear eeprom ...")); }

void clearEepromFunc() {
  menu.draw(renderClearEeprom);
  storage.clearEeprom();
  menu.clear();
}