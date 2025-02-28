#include "stepperUtils.h"

void StepperUtils::driveToEndstop(uint8_t axis, int8_t dir) {
    steppers[axis].setSpeed(dir * HOMING_SPEED);
    while (!stopStepper[axis]) {
        steppers[axis].runSpeed();
    }
    steppers[axis].setCurrentPosition(0);
}

void StepperUtils::stopAll() {
    for (AccelStepper &stepper : steppers) {
        stepper.stop();
    }
}

void StepperUtils::moveTo(uint8_t axis, int dest) {
    steppers[axis].moveTo(dest);
}

void StepperUtils::makeSteps(uint8_t axis, int dest) {
    steppers[axis].moveTo(dest);
    while (!destinationReached(axis) && !stopStepper[axis]) {
        steppers[axis].run();
    }
}

void StepperUtils::run(uint8_t axis) {
    steppers[axis].run();
}

void StepperUtils::step(long pos[]) {
    allSteppers.moveTo(pos);
}

void StepperUtils::performStepSequence(Item *positions[]) {
    long destinations[NUM_STEPPERS];
    Item current;
    int16_t currVal;

    for (uint8_t i = 0; i < NUM_STEPPERS; i++) {
        destinations[i] = steppers[i].currentPosition();

        if (positions[i] == nullptr) {
            continue;
        }

        storage.loadFromProgmem(&current, positions[i]);
        storage.loadCurrentValueFromEeprom(&current, &Item::eepromOffset,
                                           currVal);
        destinations[current.axis] = currVal;
    }
    step(destinations);
    allSteppers.runSpeedToPosition();
}

bool StepperUtils::destinationReached(int axis) {
    return steppers[axis].distanceToGo() == 0;
}

void StepperUtils::initializeSteppers() {
    for (int i = 0; i < NUM_STEPPERS; i++) {
        steppers[i] = AccelStepper(motorInterfaceType, DRV_STEP[i], DRV_DIR[i]);
        steppers[i].setMaxSpeed(PROGRAM_SPEED);
        steppers[i].setAcceleration(ACCELERATION);
        allSteppers.addStepper(steppers[i]);
    }
}
