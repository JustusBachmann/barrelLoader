void findHome() {
  Serial.println("Find Home");
  driveToEndstop(&stepperX, X.endstopPin, -1);
  X0.value = 0;
  saveToEEPROM(&X0);

  driveToEndstop(&stepperY, Y.endstopPin, 1);
  Y0.value = 0;
  saveToEEPROM(&Y0);

  makeSteps(&stepperZ, 2500, -1);
  driveToEndstop(&stepperZ, Z.endstopPin, 1);
  Z0.value = 0;
  saveToEEPROM(&Z0);
}

void driveHome() {
  Serial.println("Homing");
  Position* home[] = {&X0, &Y0, &Z0};
  stepXYZ(home);
}

void placeBarrelFunc() {
  Position* pos1[] = {&Y0, &Z0};
  stepYZ(pos1);
  delay(DELAY_BETWEEN_STEPS);

  Position* pos2 = &X4;
  loadPosition(pos2);
  step(pos2);
  delay(DELAY_BETWEEN_STEPS);

  Position* pos3[] = {&Y4, &Z4};
  stepYZ(pos3);
  delay(DELAY_BETWEEN_STEPS);

  setLow(RELAIS1);
  delay(DELAY_BETWEEN_STEPS);

  Position* pos4[] = {&Y0, &Z0};
  stepYZ(pos4);
  delay(DELAY_BETWEEN_STEPS);

  Position* pos5 = &X0;
  loadPosition(pos5);
}

void barrelLoadFunc() {
  Position* pos1[] = {&X0, &Y0, &Z0};
  stepXYZ(pos1);
  delay(DELAY_BETWEEN_STEPS);
  
  setLow(RELAIS1);
  setHigh(RELAIS2);
  setLow(RELAIS3);
  makeSteps(&stepperE0, 400, 1);

  delay(1000);

  setLow(RELAIS2);
  setHigh(RELAIS3);
  delay(DELAY_BETWEEN_STEPS);

  Position* pos2 = &Y1;
  loadPosition(pos2);
  step(pos2);
  delay(DELAY_BETWEEN_STEPS);

  setHigh(RELAIS1);
  delay(DELAY_BETWEEN_STEPS);
  
  setLow(RELAIS3);
  delay(DELAY_BETWEEN_STEPS);
  
  Position* pos3[] = {&Y0, &Z1};
  stepYZ(pos3);
  delay(DELAY_BETWEEN_STEPS);

  Position* pos4[] = {&X0, &Z0};
  stepXZ(pos4);
  delay(DELAY_BETWEEN_STEPS);

  makeSteps(&stepperE0, 400, -1);
}

void peakTipFunc() {
  setHigh(MESA_IN1);
  delay(DELAY_BETWEEN_STEPS);

  setLow(MESA_OUT1);
  delay(DELAY_BETWEEN_STEPS);

  barrelLoadFunc();
  delay(DELAY_BETWEEN_STEPS);

  Position* pos1[] = {&X1, &Y2, &Z2};
  stepXYZ(pos1);
  delay(DELAY_BETWEEN_STEPS);

  // TODO request open chuck

  // Position* pos2 = &X2; // welches X2? singleSide, peak55 oder peak60? oder je ein Programm?
  // loadPosition(pos2);
  // step(pos2);
  // delay(DELAY_BETWEEN_STEPS);

  // TODO request close chuck

  setLow(RELAIS1);
  delay(DELAY_BETWEEN_STEPS);

  Position* pos3[] = {&X1, &Y0, &Z0};
  stepXYZ(pos3);
  delay(DELAY_BETWEEN_STEPS);

  setHigh(MESA_OUT1);
  delay(DELAY_BETWEEN_STEPS);

  setHigh(MESA_IN1); // ???????? ist schon high
  delay(DELAY_BETWEEN_STEPS);

  setLow(MESA_OUT1);
  delay(DELAY_BETWEEN_STEPS);

  // Position* pos4[] = {&X2, &Y2, &Z2}; // welches X2? singleSide, peak55 oder peak60? oder je ein Programm?
  // stepXYZ(pos4);
  // delay(DELAY_BETWEEN_STEPS);

  setHigh(RELAIS1);
  delay(DELAY_BETWEEN_STEPS);

  // TODO request open chuck

  Position* pos5 = &X1;
  loadPosition(pos5);
  step(pos5);
  delay(DELAY_BETWEEN_STEPS);

  placeBarrelFunc();
}