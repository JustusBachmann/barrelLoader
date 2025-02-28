#ifndef STEPPERUTILS_H
#define STEPPERUTILS_H

#include <AccelStepper.h>
#include <MultiStepper.h>
#include <menu.h>
#include <storage.h>

extern Storage storage;

class StepperUtils {
public:
  AccelStepper steppers[NUM_STEPPERS];
  MultiStepper steppersXYZ;
  volatile bool stopStepper[NUM_STEPPERS] = { false };
  const uint16_t DELAY_BETWEEN_STEPS = 1000; //1000 = 1s
  const uint8_t STEP_SIZE = 200;

  void driveToEndstop(uint8_t axis, int8_t dir) {
    // cli();
    steppers[axis].setSpeed(dir * 2500);
    while (!stopStepper[axis]) {
      steppers[axis].runSpeed();
    }
    steppers[axis].setCurrentPosition(0);

    // sei();
  }

  void stopAll() {
    for (AccelStepper stepper : steppers) {
      stepper.stop();
    }
  }

  void moveTo(uint8_t axis, int dest) { steppers[axis].moveTo(dest); }

  // void step(Item *pos[]) {
  //   long destPos[3] = {steppers[0].currentPosition(),
  //                      steppers[1].currentPosition(),
  //                      steppers[2].currentPosition()};
  //   Item item;
  //   uint8_t i = 0;
  //   while (pos[i] != nullptr) {
  //     storage.loadFromProgmem(&item, pos[i]);
  //     // Serial.println(item.pos)
  //     // destPos[item.axis] = ;
  //   }
  //   // steppersXYZ.moveTo(destPos);
  // }

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
    for (int i = 0; i < NUM_STEPPERS; i++) {
      steppers[i] = AccelStepper(motorInterfaceType, DRV_STEP[i], DRV_DIR[i]);
      steppers[i].setMaxSpeed(PROGRAM_SPEED);
      steppers[i].setAcceleration(ACCELERATION);
      
      if (i < 3) {
        steppersXYZ.addStepper(steppers[i]);
      }
    }
  }

private:
  const uint8_t motorInterfaceType = AccelStepper::DRIVER;
  const uint16_t HOMING_SPEED = 2500;
  const uint16_t PROGRAM_SPEED = 1000;
  const uint16_t ACCELERATION = 3000;
};
#endif