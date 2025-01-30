void setup() {
  Serial.begin(115200);
  pinMode(encoderCLK, INPUT_PULLUP);
  pinMode(encoderDT,  INPUT_PULLUP);
  pinMode(encoderSW,  INPUT_PULLUP);
  
  pinMode(X_DRV_ENABLE, OUTPUT);
  pinMode(Y_DRV_ENABLE, OUTPUT);
  pinMode(Z_DRV_ENABLE, OUTPUT);

  digitalWrite(X_DRV_ENABLE, LOW); 
  digitalWrite(Y_DRV_ENABLE, LOW); 
  digitalWrite(Z_DRV_ENABLE, LOW); 

  pinMode(X_ENDSTOP, INPUT_PULLUP);
  pinMode(Y_ENDSTOP, INPUT_PULLUP);
  pinMode(Z_ENDSTOP, INPUT_PULLUP);

  stepperX.setMaxSpeed(PROGRAM_SPEED);
  stepperX.setAcceleration(ACCELERATION);

  stepperY.setMaxSpeed(PROGRAM_SPEED);
  stepperY.setAcceleration(ACCELERATION);

  stepperZ.setMaxSpeed(PROGRAM_SPEED);
  stepperZ.setAcceleration(ACCELERATION);

  steppersXYZ.addStepper(stepperX);
  steppersXYZ.addStepper(stepperY);
  steppersXYZ.addStepper(stepperZ);

  steppersYZ.addStepper(stepperY);
  steppersYZ.addStepper(stepperZ);

  steppersXZ.addStepper(stepperX);
  steppersXZ.addStepper(stepperZ);

  Program homing = {"Homing", &driveHome};
  mainMenu.programs[0] = &homing;
  mainMenu.programsCount += 1;

  Program placeBarrel = {"Place Barrel", &placeBarrelFunc};
  selectProgramMenu.programs[0] = &placeBarrel;
  selectProgramMenu.programsCount += 1;
  
  Program clearEeprom = {"Clear EEPROM", &clearEEPROM};
  settingsMenu.programs[0] = &clearEeprom;
  settingsMenu.programsCount += 1;

  Program findHomePos = {"Find Home", &findHome};
  
  u8g2.begin();

  state = RUNNING;
  activeProgram = &findHomePos;
  draw(renderProgram);
  activeProgram->programFunction();
  activeProgram = nullptr;
  draw(renderMenu);
  state = IDLE;
}

void loop() {
  if (millis() - lastMillis >= delayMillis) {
    lastMillis = millis();
    if (digitalRead(encoderCLK) == LOW && !encoderCLKLowDetected) {
      encoderCLKLowDetected = true;
      encoderCLKLowTime = millis(); 
   }

    if (digitalRead(encoderDT) == LOW && !encoderDTLowDetected) {
      encoderDTLowDetected = true;
      encoderDTLowTime = millis();
    }
  }

  switch(state) {
    case RUNNING:
      return;

    case SETPOSITION:
      if (currentPosition != nullptr) {
        AccelStepper* stepper = getStepperForAxis(currentPosition->axis);
        if (digitalRead(encoderSW) == LOW) {
          waitForButtonRelease();
          while (!destinationReached(stepper)) {
            stepper->run();
          }
          saveToEEPROM();
          state = SETTING;
          draw(renderMenu);
          currentPosition = nullptr;
        }

        if (encoderCLKLowDetected && encoderDTLowDetected) {
          if (encoderCLKLowTime < encoderDTLowTime) {
            setPosition(+1);

          } else if (encoderDTLowTime < encoderCLKLowTime) {
            setPosition(-1);
          }

          resetDetection();
          draw(renderPosition);
        }
        stepper->run();
      }
      return;
      
    default:
      if (digitalRead(encoderSW) == LOW) {
        waitForButtonRelease();
        handleButtonPressedMenu();
      }

      if (encoderCLKLowDetected && encoderDTLowDetected) {
        if (encoderCLKLowTime < encoderDTLowTime) {
          clampAndScroll(+1);
        } else if (encoderDTLowTime < encoderCLKLowTime) {
          clampAndScroll(-1);
        }

        resetDetection();
        draw(renderMenu);
      }
      return;
  }
}