void findHome() {
  Serial.println("Find Home");
  currentPosition = &X0;
  driveToEndstop(&stepperX, X.endstopPin, -1);
  currentPosition->value = 0;
  saveToEEPROM();

  currentPosition = &Y0;
  driveToEndstop(&stepperY, Y.endstopPin, 1);
  currentPosition->value = 0;
  saveToEEPROM();

  currentPosition = &Z0;
  driveToEndstop(&stepperZ, Z.endstopPin, 1);
  currentPosition->value = 0;
  saveToEEPROM();
  Serial.println("Found Home");
}

void driveHome() {
  Serial.println("Homing");
  Position* home[] = {&X0, &Y0, &Z0};
  stepXYZ(home);
}

void placeBarrelFunc() {
  Position* pos1[] = {&Y0, &Z0};
  stepYZ(pos1);
  delay(1000);
  Position* pos2 = &X4;
  loadPosition(pos2);
  step(pos2);
  delay(1000);
  Position* pos3[] = {&Y4, &Z4};
  stepYZ(pos3);
  delay(1000);
  Position* pos4[] = {&Y0, &Z0};
  stepYZ(pos4);
  delay(1000);
  Position* pos5 = &X0;
  loadPosition(pos5);
  step(pos5);
}

