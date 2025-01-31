void setup() {
  Serial.begin(115200);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT,  INPUT_PULLUP);
  pinMode(ENCODER_SW,  INPUT_PULLUP);
  
  pinMode(X_DRV_ENABLE, OUTPUT);
  pinMode(Y_DRV_ENABLE, OUTPUT);
  pinMode(Z_DRV_ENABLE, OUTPUT);
  pinMode(E0_DRV_ENABLE, OUTPUT);
  pinMode(E1_DRV_ENABLE, OUTPUT);

  pinMode(X_ENDSTOP, INPUT_PULLUP);
  pinMode(Y_ENDSTOP, INPUT_PULLUP);
  pinMode(Z_ENDSTOP, INPUT_PULLUP);
  pinMode(E0_ENDSTOP, INPUT_PULLUP);

  pinMode(MESA_OUT1, OUTPUT);
  pinMode(MESA_OUT2, OUTPUT);
  pinMode(MESA_OUT3, OUTPUT);
  pinMode(RELAIS1, OUTPUT);
  pinMode(RELAIS2, OUTPUT);
  pinMode(RELAIS3, OUTPUT);

  pinMode(MESA_IN1, INPUT);
  pinMode(MESA_IN2, INPUT);
  pinMode(MESA_IN3, INPUT);
  pinMode(MESA_IN4, INPUT);

  setLow(X_DRV_ENABLE);
  setLow(Y_DRV_ENABLE);
  setLow(Z_DRV_ENABLE);

  stepperX.setMaxSpeed(PROGRAM_SPEED);
  stepperX.setAcceleration(ACCELERATION);

  stepperY.setMaxSpeed(PROGRAM_SPEED);
  stepperY.setAcceleration(ACCELERATION);

  stepperZ.setMaxSpeed(PROGRAM_SPEED);
  stepperZ.setAcceleration(ACCELERATION);

  stepperE0.setMaxSpeed(PROGRAM_SPEED);
  stepperE0.setAcceleration(ACCELERATION);

  stepperE1.setMaxSpeed(PROGRAM_SPEED);
  stepperE1.setAcceleration(ACCELERATION);

  steppersXYZ.addStepper(stepperX);
  steppersXYZ.addStepper(stepperY);
  steppersXYZ.addStepper(stepperZ);

  steppersYZ.addStepper(stepperY);
  steppersYZ.addStepper(stepperZ);

  steppersXZ.addStepper(stepperX);
  steppersXZ.addStepper(stepperZ);

  Program homing = {"Homing", &driveHome, NONE};
  mainMenu.programs[0] = &homing;
  mainMenu.programsCount += 1;

  Program placeBarrel = {"Place Barrel", &placeBarrelFunc, NONE};
  selectProgramMenu.programs[0] = &placeBarrel;
  selectProgramMenu.programsCount += 1;

  Program loadBarrel = {"Load Barrel", &barrelLoadFunc, NONE};
  selectProgramMenu.programs[1] = &loadBarrel;
  selectProgramMenu.programsCount += 1;

  Program peak55Tip = {"Peak 55 Tip", &peakTipFunc, PEAK_55};
  selectProgramMenu.programs[2] = &peak55Tip;
  selectProgramMenu.programsCount += 1;

  Program peak60Tip = {"Peak 60 Tip", &peakTipFunc, PEAK_60};
  selectProgramMenu.programs[3] = &peak60Tip;
  selectProgramMenu.programsCount += 1;

  Program peakSiSiTip = {"Peak Single Side", &peakTipFunc, SINGLE_SIDE};
  selectProgramMenu.programs[4] = &peakSiSiTip;
  selectProgramMenu.programsCount += 1;
  
  Program clearEeprom = {"Clear EEPROM", &clearEEPROM, NONE};
  settingsMenu.programs[0] = &clearEeprom;
  settingsMenu.programsCount += 1;

  Program findHomePos = {"Find Home", &findHome, NONE};
  
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
  if (millis() - lastMillis >= DELAY_READ_ENCODER) {
    lastMillis = millis();
    if (digitalRead(ENCODER_CLK) == LOW && !encoderCLKLowDetected) {
      encoderCLKLowDetected = true;
      encoderCLKLowTime = millis(); 
   }

    if (digitalRead(ENCODER_DT) == LOW && !encoderDTLowDetected) {
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
        if (digitalRead(ENCODER_SW) == LOW) {
          waitForButtonRelease();
          while (!destinationReached(stepper)) {
            stepper->run();
          }
          saveNewPosition();
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
      if (digitalRead(ENCODER_SW) == LOW) {
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