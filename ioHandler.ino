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

void setPosition(int8_t dir) {
  newPosition.value += STEP_SIZE * dir;
  DynPosition* positions[1];
  positions[0] = {&newPosition};
  step(positions, 1);
}

void saveNewPosition() {
  if (currentPosition.pos != nullptr) {
    currentPosition.value = newPosition.value;
    saveToEeprom(&currentPosition);
  }
  state = State::IDLE;
  draw(renderMenu);
}


void handleButtonPressedMenu() {
  uint8_t selectedItemType = determineSelectedItemType();
  
  switch (selectedItemType) {
    case 0:
      navigateToSubMenu(selectedIndex);
      break;
    case 1:
      processPositionItem(selectedIndex - activePage.subMenusCount);
      break;
    case 2:
      processProgramItem(selectedIndex - activePage.subMenusCount - activePage.positionsCount);
      break;
    default:
      goBack();
      draw(renderMenu);
      break;
  }
}

uint8_t determineSelectedItemType() {
    if (selectedIndex < activePage.subMenusCount) {
        return 0;
    } else if (selectedIndex < activePage.subMenusCount + activePage.positionsCount) {
        return 1;
    } else if (selectedIndex < activePage.subMenusCount + activePage.positionsCount + activePage.programsCount) {
        return 2;
    }
    return 3;
}

void navigateToSubMenu(uint8_t index) {
    const MenuPage* subMenuPtr = getFromProgmem(activePage.subMenus, index);
    changePage(subMenuPtr);
    draw(renderMenu);
}

void processPositionItem(int positionIndex) {
    if (checkCrc()) {
        const Position* positionPtr = getFromProgmem(activePage.positions, positionIndex);
        loadCurrentPosFromEeprom(positionPtr);
        draw(renderPosition);
        state = State::SETPOSITION;
        DynPosition* positions[1];
        positions[0] = {&currentPosition};
        step(positions, 1);
    } else {
        draw(renderMenu);
    }
}

void processProgramItem(int programIndex) {
    const Program* programPtr = getFromProgmem(activePage.programs, programIndex);
    if (checkCrc() || programPtr == &clearEepromProg) {
        state = State::RUNNING;
        Program activeProgram;
        loadFromProgmem(&activeProgram, programPtr);
        draw(renderProgram);
        printFromProgmem(activeProgram.name);
        peak = activeProgram.peakMode;
        activeProgram.programFunction();
        state = State::IDLE;
    }
    draw(renderMenu);
}

// void handleButtonPressedMenu() {
//     if (selectedIndex < activePage.subMenusCount) {
//       const MenuPage* subMenuPtr = getFromProgmem(activePage.subMenus, selectedIndex);
//       changePage(subMenuPtr);
//       draw(renderMenu);
//       return;
//     }

//     int selectedPositionItem = selectedIndex - activePage.subMenusCount;
//     if (selectedPositionItem < activePage.positionsCount) {
//       if (checkCrc()) {
//         const Position* positionPtr = getFromProgmem(activePage.positions, selectedPositionItem);
//         loadCurrentPosFromEeprom(positionPtr);
//         draw(renderPosition);
//         state = State::SETPOSITION;
//         step(currentPosition.axis, currentPosition.value);
//       }
//       return;
//     }

//     int selectedProgramItem = selectedPositionItem - activePage.positionsCount;
//     if (selectedProgramItem < activePage.programsCount) {
//       const Program* programPtr = getFromProgmem(activePage.programs, selectedProgramItem);
//       if (checkCrc() || programPtr == &clearEepromProg) {
//         state = State::RUNNING;
//         Program activeProgram;
//         loadFromProgmem(&activeProgram, programPtr);
//         printFromProgmem(activeProgram.name);
//         draw(renderProgram);
//         activeProgram.programFunction();
//         draw(renderMenu);
//         activeProgram = {};
//         state = State::IDLE;
//       }
//       return;
//     }

//     goBack();
//     draw(renderMenu);
// }
