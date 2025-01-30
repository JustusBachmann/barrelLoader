void driveToEndstop(AccelStepper* stepper, int endstopPin, int dir) {
  stepper->setSpeed(dir * HOMING_SPEED);
  while (digitalRead(endstopPin) == HIGH) {
    stepper->runSpeed();
  }
  stepper->stop();
  stepper->setCurrentPosition(0);
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

void step(Position* pos) {
  AccelStepper* stepper = getStepperForAxis(pos->axis);

  stepper->moveTo(pos->value);
  stepper->runToPosition();
}

AccelStepper* getStepperForAxis(Axis* axis) {
  switch(axis->axis) {
    case X_AXIS:
      return &stepperX;
    case Y_AXIS:
      return &stepperY;
    case Z_AXIS:
      return &stepperZ;
  }
}