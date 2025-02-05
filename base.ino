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

  u8g2.begin();
  loadActivePage(&mainMenu);
  Program activeProgram;
  draw(renderProgram);
  loadProgramFromProgmem(&activeProgram, &clearEepromProg);
  activeProgram.programFunction();

  initializeSteppers();
  draw(renderMenu);
  state = State::IDLE;
}

void loop() {
  delay(1);
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
    case State::IDLE:
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

    case State::SETPOSITION:
      int stepperID = getIntFromAxis(currentPosition.pos->axis);
      if (digitalRead(ENCODER_SW) == LOW) {
        waitForButtonRelease();
        while (!destinationReached(stepperID)) {
          steppers[stepperID].run();
        }
        saveNewPosition();
      }

      if (encoderState.clkLowDetected && encoderState.dtLowDetected) {
        if (encoderState.clkLowTime < encoderState.dtLowTime) {
          setPosition(currentPosition.pos->axis, +1);

        } else if (encoderState.dtLowTime < encoderState.clkLowTime) {
          setPosition(currentPosition.pos->axis, -1);
        }

        resetDetection();
        draw(renderPosition);
      }
      // if ()
      steppers[stepperID].run();
      return;
      
    default:
      return;
  }
}
