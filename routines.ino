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
  Position* X2;
  Position* X3;

  switch(activeProgram->peakMode) {
    case PEAK_55:
      X2 = &peak55X2;
      X3 = &peak55X3;
      break;

    case PEAK_60:
      X2 = &peak60X2;
      X3 = &peak60X3;
      break;

    case SINGLE_SIDE:
      X2 = &singleSideX2;
      break;

    default:
      Serial.println("no peak configured");
      return;
  }

  setHigh(MESA_IN1);
  Serial.println("Mesa in 1 high");
  delay(DELAY_BETWEEN_STEPS);

  setLow(MESA_OUT1);
  Serial.println("Mesa out 1 low");
  delay(DELAY_BETWEEN_STEPS);

  barrelLoadFunc();
  Serial.println("barrel loaded");
  delay(DELAY_BETWEEN_STEPS);

  Position* pos1[] = {&X1, &Y2, &Z2};
  stepXYZ(pos1);
  Serial.println("drived position 1");
  delay(DELAY_BETWEEN_STEPS);

  // TODO request open chuck

  Position* pos2 = X2;
  loadPosition(pos2);
  step(pos2);
  Serial.println("drived position 2");
  delay(DELAY_BETWEEN_STEPS);

  // TODO request close chuck

  setLow(RELAIS1);
  Serial.println("relais 1 low");
  delay(DELAY_BETWEEN_STEPS);

  Position* pos3[] = {&X1, &Y0, &Z0};
  stepXYZ(pos3);
  Serial.println("drived position 3");
  delay(DELAY_BETWEEN_STEPS);

  setHigh(MESA_OUT1);
  Serial.println("Mesa out 1 high");
  delay(DELAY_BETWEEN_STEPS);

  setHigh(MESA_IN1); 
  Serial.println("Mesa in 1 high");
  delay(DELAY_BETWEEN_STEPS);

  setLow(MESA_OUT1);
  Serial.println("Mesa out 1 low");
  delay(DELAY_BETWEEN_STEPS);

  Position* pos4[] = {X2, &Y2, &Z2};
  stepXYZ(pos4);
  Serial.println("drived position 4");
  delay(DELAY_BETWEEN_STEPS);

  setHigh(RELAIS1);
  Serial.println("relais 1 high");
  delay(DELAY_BETWEEN_STEPS);

  // TODO request open chuck

  Position* pos5 = &X1;
  loadPosition(pos5);
  step(pos5);
  Serial.println("drived position 5");
  delay(DELAY_BETWEEN_STEPS);

  placeBarrelFunc();
  Serial.println("barrel placed");
}