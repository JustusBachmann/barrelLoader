void resetDetection() {
  encoderCLKLowDetected = false;
  encoderDTLowDetected = false;
  encoderCLKLowTime = 0;
  encoderDTLowTime = 0;
}


void waitForButtonRelease() {
  const unsigned long debounceDelay = 50;
  unsigned long lastDebounceTime = 0;

  while (true) {
    bool currentState = digitalRead(encoderSW);
    if (currentState == HIGH) {
      if ((millis() - lastDebounceTime) > debounceDelay) {
        break;
      }
    } else {
      lastDebounceTime = millis();
    }
  }

  buttonPressed = false;
}

void handleButtonPressedMenu() {
    if (selectedIndex < activePage->subMenusCount) {
      changePage(activePage->subMenus[selectedIndex]);
      draw(renderMenu);
      return;
    }
    int selectedPositionItem = selectedIndex - activePage->subMenusCount;
    if (selectedPositionItem < activePage->positionsCount) {
      readPositionFromEEPROM(activePage->positions[selectedIndex - activePage->subMenusCount]);
      draw(renderPosition);
      if (state == SETTING) {
        state = SETPOSITION;
      }
      return;
    }

    int selectedMenuItem = selectedIndex - activePage->subMenusCount - activePage->positionsCount;
    if (strcmp(activePage->items[selectedMenuItem], "Back") == 0) {
      goBack();
      draw(renderMenu);
      return;
    }
    

    // Handle other actions here
    Serial.print("Selected action: ");
    Serial.println(activePage->items[selectedMenuItem]);
}

void setPosition(int dir) {
  newPosition += stepSize * dir;
  switch(currentPosition->axis) {
    case X:
      stepperX.move(stepSize * dir);
      stepperX.runToPosition();
      break;
    case Y:
      stepperY.move(stepSize * dir);
      stepperY.runToPosition();
      break;
    case Z:
      stepperZ.move(stepSize * dir);
      stepperZ.runToPosition();
      break;
  }
  
}
