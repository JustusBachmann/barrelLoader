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

bool destinationReached(AccelStepper* stepper) {
  if (stepper->distanceToGo() == 0) {
    return true;
  }
  return false;
}

void handleButtonPressedMenu() {
    if (selectedIndex < activePage->subMenusCount) {
      changePage(activePage->subMenus[selectedIndex]);
      draw(renderMenu);
      return;
    }

    int selectedPositionItem = selectedIndex - activePage->subMenusCount;
    if (selectedPositionItem < activePage->positionsCount) {
      readPositionFromEEPROM(activePage->positions[selectedPositionItem]);
      if (state == SETTING) {
        draw(renderPosition);
        state = SETPOSITION;
        step(currentPosition);
      }
      return;
    }

    int selectedProgramItem = selectedPositionItem - activePage->positionsCount;
    if (selectedProgramItem < activePage->programsCount) {
      state = RUNNING;
      activeProgram = activePage->programs[selectedProgramItem];
      draw(renderProgram);
      activeProgram->programFunction();
      draw(renderMenu);
      activeProgram = nullptr;
      state = IDLE;
      return;
    }

    int selectedMenuItem = selectedProgramItem - activePage->programsCount;
    if (strcmp(activePage->items[selectedMenuItem], "Back") == 0) {
      goBack();
      draw(renderMenu);
      return;
    }
}
