void resetDetection() {
  encoderState = {0, 0, false, false, false};
}

void setHigh(int pin) {
  digitalWrite(pin, HIGH);
}

void setLow(int pin) {
  digitalWrite(pin, LOW);
}

void waitForButtonRelease() {
  const unsigned long debounceDelay = 50;
  unsigned long lastDebounceTime = 0;

  while (true) {
    bool currentState = digitalRead(ENCODER_SW);
    if (currentState == HIGH) {
      if ((millis() - lastDebounceTime) > debounceDelay) {
        break;
      }
    } else {
      lastDebounceTime = millis();
    }
  }
  encoderState.buttonPressed = false;
}

void saveNewPosition() {
  if (currentPosition.pos != nullptr) {
    currentPosition.value = newPosition.value;
    // saveToEEPROM(currentPosition);
    currentPosition = {nullptr, 0};
    state = State::IDLE;
    draw(renderMenu);
  }
}

void handleButtonPressedMenu() {
    if (selectedIndex < activePage.subMenusCount) {
      const MenuPage* subMenuPtr = getSubMenuFromProgmem(activePage.subMenus, selectedIndex);
      changePage(subMenuPtr);
      draw(renderMenu);
      return;
    }

    int selectedPositionItem = selectedIndex - activePage.subMenusCount;
    if (selectedPositionItem < activePage.positionsCount) {
      // readPositionFromEEPROM(activePage.positions[selectedPositionItem]);
      draw(renderPosition);
      state = State::SETPOSITION;
      // step(currentPosition);
      return;
    }

    int selectedProgramItem = selectedPositionItem - activePage.positionsCount;
    if (selectedProgramItem < activePage.programsCount) {
      if (checkCRC() || strcmp(activePage.programs[selectedProgramItem]->name, "Clear EEPROM") == 0) {
        state = State::RUNNING;
        activeProgram = activePage.programs[selectedProgramItem];
        draw(renderProgram);
        activeProgram->programFunction();
        draw(renderMenu);
        activeProgram = nullptr;
        state = State::IDLE;
      }
      return;
    }

    goBack();
    draw(renderMenu);
}
