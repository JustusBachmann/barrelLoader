void driveToEndstop(uint8_t axis, int8_t dir) {
  steppers[axis].setSpeed(dir * HOMING_SPEED);
  while (!stop(axis)) {
    steppers[axis].runSpeed();
  }
  steppers[axis].stop();
  steppers[axis].setCurrentPosition(0);
}

bool stop(uint8_t axis) {
  return digitalRead(ENDSTOP[axis]) == LOW;
}

void makeSteps(uint8_t axis, int steps, int8_t dir) {
  steppers[axis].move(dir * steps);
  steppers[axis].runToPosition();
}

void step(DynPosition* pos[], uint8_t count) {
  long destPos[3] = {steppers[0].currentPosition(), steppers[1].currentPosition(), steppers[2].currentPosition()};
  for (uint8_t i = 0; i < count; i++) {
    if (pos[i] != nullptr) {
      destPos[pos[i]->axis] = pos[i]->value;
    }
  }
  steppersXYZ.moveTo(destPos);
}

void performStepSequence(Position* xPos, Position* yPos, Position* zPos, DynPosition* positions[]) {
  if (xPos) getFromEeprom(xPos, positions[0]);
  if (yPos) getFromEeprom(yPos, positions[1]);
  if (zPos) getFromEeprom(zPos, positions[2]);
  step(positions, 3);
  steppersXYZ.runSpeedToPosition();
  delay(DELAY_BETWEEN_STEPS);
}

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