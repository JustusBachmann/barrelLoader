#ifndef STEPPERUTILS_H
#define STEPPERUTILS_H

#include <AccelStepper.h>
#include <MultiStepper.h>
#include <menu.h>
#include <storage.h>
#include "config.h"

extern Storage storage;

class StepperUtils {
public:
  volatile bool stopStepper[NUM_STEPPERS] = {false};
  const uint16_t DELAY_BETWEEN_STEPS = 1000; // 1000 = 1s
  const uint8_t STEP_SIZE = 200;

  void driveToEndstop(uint8_t axis, int8_t dir);
  void stopAll();
  void moveTo(uint8_t axis, int dest);
  void makeSteps(uint8_t axis, int dest);
  void run(uint8_t axis);
  void step(long pos[]);
  void performStepSequence(Item *positions[]);
  bool destinationReached(int axis);
  void initializeSteppers();

private:
  AccelStepper steppers[NUM_STEPPERS];
  MultiStepper allSteppers;
  const uint8_t motorInterfaceType = AccelStepper::DRIVER;
  const uint16_t HOMING_SPEED = 2500;
  const uint16_t PROGRAM_SPEED = 1000;
  const uint16_t ACCELERATION = 3000;
};

#endif
