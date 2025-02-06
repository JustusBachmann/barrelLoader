void driveToEndstop(uint8_t axis, int8_t dir) {
  steppers[axis].setSpeed(dir * HOMING_SPEED);
  while (digitalRead(ENDSTOP[axis]) == HIGH) {
    steppers[axis].runSpeed();
  }
  steppers[axis].stop();
  steppers[axis].setCurrentPosition(0);
}

void step(uint8_t axis, int16_t dest) { 
  steppers[axis].moveTo(dest);
  steppers[axis].runSpeedToPosition();
}

void makeSteps(uint8_t axis, int steps, int8_t dir) {
  steppers[axis].move(dir * steps);
  steppers[axis].runToPosition();
}

void stepXYZ(DynPosition* pos[3]) {
  long destPos[3];
  destPos[0] = pos[0]->value;
  destPos[1] = pos[1]->value;
  destPos[2] = pos[2]->value;
  steppersXYZ.moveTo(destPos);
  steppersXYZ.runSpeedToPosition();
}

void stepYZ(DynPosition* pos[2]) {
  long destPos[2];
  destPos[0] = pos[0]->value;
  destPos[1] = pos[1]->value;
  steppersYZ.moveTo(destPos);
  steppersYZ.runSpeedToPosition();
}

void stepXZ(DynPosition* pos[2]) {
  long destPos[2];
  destPos[0] = pos[0]->value;
  destPos[1] = pos[1]->value;
  steppersXZ.moveTo(destPos);
  steppersXZ.runSpeedToPosition();
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

  steppersYZ.addStepper(steppers[1]);
  steppersYZ.addStepper(steppers[2]);

  steppersXZ.addStepper(steppers[0]);
  steppersXZ.addStepper(steppers[1]);

}