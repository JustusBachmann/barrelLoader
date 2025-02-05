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

void setPosition(Axis axis, int8_t dir) {
  newPosition += STEP_SIZE * dir;
  step(axis, newPosition);
}

void saveNewPosition() {
  if (currentPosition.pos != nullptr) {
    currentPosition.value = newPosition;
    saveToEeprom(&currentPosition);
  }
  currentPosition = {nullptr, 0};
  state = State::IDLE;
  draw(renderMenu);
}

void handleButtonPressedMenu() {
    if (selectedIndex < activePage.subMenusCount) {
      const MenuPage* subMenuPtr = getFromProgmem(activePage.subMenus, selectedIndex);
      changePage(subMenuPtr);
      draw(renderMenu);
      return;
    }

    int selectedPositionItem = selectedIndex - activePage.subMenusCount;
    if (selectedPositionItem < activePage.positionsCount) {
      if (checkCrc()) {
        const Position* positionPtr = getFromProgmem(activePage.positions, selectedPositionItem);
        loadCurrentPosFromEeprom(positionPtr);
        draw(renderPosition);
        state = State::SETPOSITION;
        step(currentPosition.pos->axis, currentPosition.value);
      }
      return;
    }

    int selectedProgramItem = selectedPositionItem - activePage.positionsCount;
    if (selectedProgramItem < activePage.programsCount) {
      const Program* programPtr = getFromProgmem(activePage.programs, selectedProgramItem);
      if (checkCrc() || programPtr == &clearEepromProg) {
        state = State::RUNNING;
        Program activeProgram;
        loadProgramFromProgmem(&activeProgram, programPtr);
        printFromProgmem(activeProgram.name);
        draw(renderProgram);
        activeProgram.programFunction();
        draw(renderMenu);
        activeProgram = {};
        state = State::IDLE;
      }
      return;
    }

    goBack();
    draw(renderMenu);
}
