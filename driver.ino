void driveToEndstop(Axis axis, int8_t dir) {
  int stepperID = getIntFromAxis(axis);
  steppers[stepperID].setSpeed(dir * HOMING_SPEED);
  while (digitalRead(ENDSTOP[stepperID]) == HIGH) {
    steppers[stepperID].runSpeed();
  }
  steppers[stepperID].stop();
  steppers[stepperID].setCurrentPosition(0);
}

void step(Axis axis, int16_t dest) {
  int stepperID = getIntFromAxis(axis); 
  Serial.println(stepperID);
  steppers[stepperID].moveTo(dest);
  steppers[stepperID].runSpeedToPosition();
}

// void makeSteps(AccelStepper* stepper, int steps, int8_t dir) {
//   stepper->move(dir * steps);
//   stepper->runToPosition();
// }

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

int getIntFromAxis(Axis axis) {
  switch(axis) {
    case Axis::X:
      return 0;
    case Axis::Y:
      return 1;
    case Axis::Z:
      return 2;
  }
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