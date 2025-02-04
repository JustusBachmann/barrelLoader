void setup() {
  Serial.begin(115200);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT,  INPUT_PULLUP);
  pinMode(ENCODER_SW,  INPUT_PULLUP);
  
  for (uint8_t i = 0; i < 5; i++) {
    pinMode(DRV_ENABLE[i], OUTPUT);
    setLow(DRV_ENABLE[i]);
  }

  for (uint8_t i = 0; i < 4; i++) {
    pinMode(ENDSTOP[i], INPUT_PULLUP);
  }
  
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(MESA_OUT[i], OUTPUT);
  }
   
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(RELAIS[i], OUTPUT);
  }
  
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(MESA_IN[i], INPUT);
  }

  initializeSteppers();


  Program homing = {"Homing", &findHome, Peak::NONE};
  mainMenu.programs[0] = &homing;
  mainMenu.programsCount += 1;

  // Program placeBarrel = {"Place Barrel", &placeBarrelFunc, NONE};
  // selectProgramMenu.programs[0] = &placeBarrel;
  // selectProgramMenu.programsCount += 1;

  // Program loadBarrel = {"Load Barrel", &barrelLoadFunc, NONE};
  // selectProgramMenu.programs[1] = &loadBarrel;
  // selectProgramMenu.programsCount += 1;

  Program peak55Tip = {"Peak 55 Tip", &peakTipFunc, Peak::PEAK_55};
  selectProgramMenu.programs[0] = &peak55Tip;
  selectProgramMenu.programsCount += 1;

  Program peak60Tip = {"Peak 60 Tip", &peakTipFunc, Peak::PEAK_60};
  selectProgramMenu.programs[1] = &peak60Tip;
  selectProgramMenu.programsCount += 1;

  Program peakSiSiTip = {"Peak Single Side", &peakTipFunc, Peak::SINGLE_SIDE};
  selectProgramMenu.programs[2] = &peakSiSiTip;
  selectProgramMenu.programsCount += 1;
  
  Program clearEeprom = {"Clear EEPROM", &clearEEPROM, Peak::NONE};
  settingsMenu.programs[0] = &clearEeprom;
  settingsMenu.programsCount += 1; 

  u8g2.begin();

  draw(renderMenu);
  state = State::IDLE;
}

void loop() {
  if (millis() - lastMillis >= DELAY_READ_ENCODER) {
    lastMillis = millis();
    if (digitalRead(ENCODER_CLK) == LOW && !encoderState.clkLowDetected) {
      encoderState.clkLowDetected = true;
      encoderState.clkLowTime = millis(); 
   }

    if (digitalRead(ENCODER_DT) == LOW && !encoderState.dtLowDetected) {
      encoderState.dtLowDetected = true;
      encoderState.dtLowTime = millis();
    }
  }

  switch(state) {
    case State::STOP:
      return;
    case State::RUNNING:
      return;

    case State::SETPOSITION:
      if (currentPosition != nullptr) {
        AccelStepper* stepper = getStepperForAxis(currentPosition->axis);
        if (digitalRead(ENCODER_SW) == LOW) {
          waitForButtonRelease();
          while (!destinationReached(stepper)) {
            stepper->run();
          }
          saveNewPosition();
        }

        if (encoderState.clkLowDetected && encoderState.dtLowDetected) {
          if (encoderState.clkLowTime < encoderState.dtLowTime) {
            setPosition(+1);

          } else if (encoderState.dtLowTime < encoderState.clkLowTime) {
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

       if (encoderState.clkLowDetected && encoderState.dtLowDetected) {
          if (encoderState.clkLowTime < encoderState.dtLowTime) {
          clampAndScroll(+1);
          } else if (encoderState.dtLowTime < encoderState.clkLowTime) {
          clampAndScroll(-1);
        }

        resetDetection();
        draw(renderMenu);
      }
      return;
  }
}
