void findHome() {
  loadCurrentPosFromEeprom(&X0);
  driveToEndstop(Axis::X, -1);
  currentPosition.value = 0;
  saveToEeprom(&currentPosition);

  loadCurrentPosFromEeprom(&Y0);
  driveToEndstop(Axis::Y, 1);
  currentPosition.value = 0;
  saveToEeprom(&currentPosition);

  // makeSteps(&steppers[2], 2500, -1);
  loadCurrentPosFromEeprom(&Z0);
  driveToEndstop(Axis::Z, 1);
  currentPosition.value = 0;
  saveToEeprom(&currentPosition);
}

void driveHome() {
  // Position* home[] = {&X0, &Y0, &Z0};
  // stepXYZ(home);
}

void placeBarrelFunc() {
//   Position* pos1[] = {&Y0, &Z0};
//   stepYZ(pos1);
//   delay(DELAY_BETWEEN_STEPS);

//   Position* pos2 = &X4;
//   loadPosition(pos2);
//   step(pos2);
//   delay(DELAY_BETWEEN_STEPS);

//   Position* pos3[] = {&Y4, &Z4};
//   stepYZ(pos3);
//   delay(DELAY_BETWEEN_STEPS);

//   setLow(RELAIS[0]);
//   delay(DELAY_BETWEEN_STEPS);

//   Position* pos4[] = {&Y0, &Z0};
//   stepYZ(pos4);
//   delay(DELAY_BETWEEN_STEPS);

//   Position* pos5 = &X0;
//   loadPosition(pos5);
}

void barrelLoadFunc() {
  // Position* pos1[] = {&X0, &Y0, &Z0};
  // stepXYZ(pos1);
  // delay(DELAY_BETWEEN_STEPS);
  
  // setLow(RELAIS[0]);
  // setHigh(RELAIS[1]);
  // setLow(RELAIS[2]);
  // makeSteps(&steppers[3], 400, 1);

  // delay(1000);

  // setLow(RELAIS[1]);
  // setHigh(RELAIS[2]);
  // delay(DELAY_BETWEEN_STEPS);

  // Position* pos2 = &Y1;
  // loadPosition(pos2);
  // step(pos2);
  // delay(DELAY_BETWEEN_STEPS);

  // setHigh(RELAIS[0]);
  // delay(DELAY_BETWEEN_STEPS);
  
  // setLow(RELAIS[2]);
  // delay(DELAY_BETWEEN_STEPS);
  
  // Position* pos3[] = {&Y0, &Z1};
  // stepYZ(pos3);
  // delay(DELAY_BETWEEN_STEPS);

  // Position* pos4[] = {&X0, &Z0};
  // stepXZ(pos4);
  // delay(DELAY_BETWEEN_STEPS);

  // makeSteps(&steppers[3], 400, -1);
}

void peakTipFunc() {
  // Position* X2;
  // Position* X3;

  // switch(activeProgram->peakMode) {
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
  // delay(DELAY_BETWEEN_STEPS);

  // setLow(MESA_OUT[0]);
  // delay(DELAY_BETWEEN_STEPS);

  // barrelLoadFunc();
  // delay(DELAY_BETWEEN_STEPS);

  // Position* pos1[] = {&X1, &Y2, &Z2};
  // stepXYZ(pos1);
  // delay(DELAY_BETWEEN_STEPS);

  // // TODO request open chuck

  // Position* pos2 = X2;
  // loadPosition(pos2);
  // step(pos2);
  // delay(DELAY_BETWEEN_STEPS);

  // // TODO request close chuck

  // setLow(RELAIS[0]);
  // delay(DELAY_BETWEEN_STEPS);

  // Position* pos3[] = {&X1, &Y0, &Z0};
  // stepXYZ(pos3);
  // delay(DELAY_BETWEEN_STEPS);

  // setHigh(MESA_OUT[0]);
  // delay(DELAY_BETWEEN_STEPS);

  // setHigh(MESA_IN[0]); 
  // delay(DELAY_BETWEEN_STEPS);

  // setLow(MESA_OUT[0]);
  // delay(DELAY_BETWEEN_STEPS);

  // Position* pos4[] = {X2, &Y2, &Z2};
  // stepXYZ(pos4);
  // delay(DELAY_BETWEEN_STEPS);

  // setHigh(RELAIS[0]);
  // delay(DELAY_BETWEEN_STEPS);

  // // TODO request open chuck

  // Position* pos5 = &X1;
  // loadPosition(pos5);
  // step(pos5);
  // delay(DELAY_BETWEEN_STEPS);

  // placeBarrelFunc();
}
