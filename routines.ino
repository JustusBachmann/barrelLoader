void findHome() {
  loadCurrentPosFromEeprom(&X0);
  driveToEndstop(0, -1);
  currentPosition.value = 0;
  saveToEeprom(&currentPosition);

  loadCurrentPosFromEeprom(&Y0);
  driveToEndstop(1, 1);
  currentPosition.value = 0;
  saveToEeprom(&currentPosition);

  makeSteps(2, 2500, -1);
  loadCurrentPosFromEeprom(&Z0);
  driveToEndstop(2, 1);
  currentPosition.value = 0;
  saveToEeprom(&currentPosition);
}

void placeBarrelFunc(DynPosition* xDynPos, DynPosition* yDynPos, DynPosition* zDynPos) {
  DynPosition* positions[3] = {xDynPos, yDynPos, zDynPos};

  performStepSequence(nullptr, &Y0, &Z0, positions);

  performStepSequence(&X4, nullptr, nullptr, positions);

  performStepSequence(nullptr, &Y4, &Z4, positions);

  setLow(RELAIS[0]);

  performStepSequence(nullptr, &Y0, &Z0, positions);

  performStepSequence(&X0, nullptr, nullptr, positions);
}

void barrelLoadFunc(DynPosition* xDynPos, DynPosition* yDynPos, DynPosition* zDynPos) {
  DynPosition* positions[3] = {xDynPos, yDynPos, zDynPos};

  performStepSequence(&X0, &Y0, &Z0, positions);
  
  setLow(RELAIS[0]);
  setHigh(RELAIS[1]);
  setLow(RELAIS[2]);

  makeSteps(3, 400, 1);
 
  delay(1000);
 
  setLow(RELAIS[1]);
  setHigh(RELAIS[2]);
  
  performStepSequence(nullptr, &Y1, nullptr, positions);

  setHigh(RELAIS[0]);
  setLow(RELAIS[2]);

  performStepSequence(nullptr, &Y0, &Z1, positions);

  performStepSequence(&X0, nullptr, &Z0, positions);
  
  makeSteps(3, 400, -1);
}


void peakTipFunc() {
  DynPosition xDynPos, yDynPos, zDynPos;
  DynPosition* positions[3] = {&xDynPos, &yDynPos, &zDynPos};
  
  Position* X2 = nullptr;
  Position* X3 = nullptr;

  switch(peak) {
    case Peak::PEAK_55:
      X2 = &peak55X2;
      X3 = &peak55X3;
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

  setHigh(MESA_IN[0]);
  
  setLow(MESA_OUT[0]);

  barrelLoadFunc(&xDynPos, &yDynPos, &zDynPos);

  performStepSequence(&X1, &Y2, &Z2, positions);

  // TODO request open chuck
  
  performStepSequence(X2, nullptr, nullptr, positions);
  
  // TODO request close chuck

  setLow(RELAIS[0]);

  performStepSequence(&X1, &Y0, &Z0, positions);
  
  setHigh(MESA_OUT[0]);
  setHigh(MESA_IN[0]); 
  setLow(MESA_OUT[0]);
  
  performStepSequence(X2, &Y2, &Z2, positions);
  
  setHigh(RELAIS[0]);
  
  // TODO request open chuck

  performStepSequence(&X1, nullptr, nullptr, positions);

  placeBarrelFunc(&xDynPos, &yDynPos, &zDynPos);
}
