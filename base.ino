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

  // initializeSteppers();
  loadActivePage(&mainMenu);
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
      if (currentPosition.pos != nullptr) {
        // AccelStepper* stepper = getStepperForAxis(currentPosition->axis);
        if (digitalRead(ENCODER_SW) == LOW) {
          waitForButtonRelease();
          // while (!destinationReached(stepper)) {
          //   stepper->run();
          // }
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
        // stepper->run();
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
