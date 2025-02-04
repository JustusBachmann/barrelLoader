void driveToEndstop(AccelStepper* stepper, int endstopPin, int dir) {
  stepper->setSpeed(dir * HOMING_SPEED);
  while (digitalRead(endstopPin) == HIGH) {
    stepper->runSpeed();
  }
  stepper->stop();
  stepper->setCurrentPosition(0);
}

void makeSteps(AccelStepper* stepper, int steps, int dir) {
  stepper->move(dir * steps);
  stepper->runToPosition();
}

void stepXYZ(Position* pos[3]) {
  long destPos[3];
  loadAllPositions(pos, 3);
  destPos[0] = pos[0]->value;
  destPos[1] = pos[1]->value;
  destPos[2] = pos[2]->value;
  steppersXYZ.moveTo(destPos);
  steppersXYZ.runSpeedToPosition();
}

void stepYZ(Position* pos[2]) {
  long destPos[2];
  loadAllPositions(pos, 2);
  destPos[0] = pos[0]->value;
  destPos[1] = pos[1]->value;
  steppersYZ.moveTo(destPos);
  steppersYZ.runSpeedToPosition();
}

void stepXZ(Position* pos[2]) {
  long destPos[2];
  loadAllPositions(pos, 2);
  destPos[0] = pos[0]->value;
  destPos[1] = pos[1]->value;
  steppersXZ.moveTo(destPos);
  steppersXZ.runSpeedToPosition();
}

void step(Position* pos) {
  AccelStepper* stepper = getStepperForAxis(pos->axis);

  stepper->moveTo(pos->value);
  stepper->runSpeedToPosition();
}


AccelStepper* getStepperForAxis(Axis axis) {
  switch(axis) {
    case Axis::X:
      return &steppers[0];
    case Axis::Y:
      return &steppers[1];
    case Axis::Z:
      return &steppers[2];
  }
}

bool destinationReached(AccelStepper* stepper) {
  if (stepper->distanceToGo() == 0) {
    return true;
  }
  return false;
}

void initializeSteppers() {
  for (uint8_t i = 0; i < 5; i++) {
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