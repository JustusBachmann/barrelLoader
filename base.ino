void setup() {
  Serial.begin(115200);
  pinMode(encoderCLK, INPUT_PULLUP);
  pinMode(encoderDT,  INPUT_PULLUP);
  pinMode(encoderSW,  INPUT_PULLUP);

  stepperX.setMaxSpeed(1000);
  stepperX.setAcceleration(500);

  stepperY.setMaxSpeed(1000);
  stepperY.setAcceleration(500);

  stepperZ.setMaxSpeed(1000);
  stepperZ.setAcceleration(500);

  steppersXYZ.addStepper(stepperX);
  steppersXYZ.addStepper(stepperY);
  steppersXYZ.addStepper(stepperZ);

  steppersYZ.addStepper(stepperY);
  steppersYZ.addStepper(stepperZ);

  steppersXZ.addStepper(stepperX);
  steppersXZ.addStepper(stepperZ);


  u8g2.begin();
  draw(renderLoadingScreen);
  clearEEPROM();
  draw(renderMenu);
  //drive initializing stops / find Home
}

void loop() {
  if (digitalRead(encoderCLK) == LOW && !encoderCLKLowDetected) {
    encoderCLKLowDetected = true;
    encoderCLKLowTime = millis(); 
  }

  if (digitalRead(encoderDT) == LOW && !encoderDTLowDetected) {
    encoderDTLowDetected = true;
    encoderDTLowTime = millis();
  }

  switch(state) {
    case SETPOSITION:
      if (digitalRead(encoderSW) == LOW) {
        waitForButtonRelease();
        saveToEEPROM();
        state = SETTING;
        draw(renderMenu);
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

      break;
    default:
        // todo: checkEndStop
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
      break;
  }
  
}
