void clampAndScroll(int8_t direction) {
  if (selectedIndex + direction >= 0 && selectedIndex + direction < activePageLength) {
    selectedIndex += direction;
  }

  if (selectedIndex < topIndex) {
    topIndex = selectedIndex;
  } else if (selectedIndex > topIndex + (linesPerPage)) {
    topIndex = selectedIndex - (linesPerPage);
  }
}

void draw(void (*renderFunction)()) {
  u8g2.firstPage();
  do {
    renderFunction();
  } while (u8g2.nextPage());
}

void renderMenu() {
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  printStringFromProgmem(activePage.title);

  u8g2.setCursor(0, 12);
  u8g2.drawLine(0, 12, 128, 12);

  for (uint8_t i = 0; i <= linesPerPage; i++) {
    uint8_t itemIndex = topIndex + i;
    if (itemIndex >= activePageLength) break;

    uint16_t yPos = 24 + (i * 12); 

    if (itemIndex == selectedIndex) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, yPos - 10, 128, 12);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1); 
    }

    u8g2.setCursor(2, yPos);
    if (itemIndex < activePage.subMenusCount) {
      const MenuPage* subMenuPtr = getSubMenuFromProgmem(activePage.subMenus, itemIndex);
      printSubMenuFromProgmem(subMenuPtr);
    } else if (itemIndex - activePage.subMenusCount < activePage.positionsCount) {
      const Position* positionPtr = getPositionFromProgmem(activePage.positions, itemIndex - activePage.subMenusCount - activePage.positionsCount);
      printPositionFromProgmem(positionPtr);
    } else if (itemIndex - activePage.subMenusCount - activePage.positionsCount < activePage.programsCount) {
      const Program* programPtr = getProgramFromProgmem(activePage.programs, itemIndex - activePage.subMenusCount);
      printProgramFromProgmem(programPtr);
    } else if (activePage.back != nullptr) {
      printStringFromProgmem(activePage.back);
    }
  }

  u8g2.setDrawColor(1);
}

void renderLoadingScreen() {
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawCircle(64, 32, 20);
  u8g2.drawLine(64, 27, 64, 37);
  u8g2.drawLine(59, 32, 69, 32);
  u8g2.setCursor(40, 55);
  u8g2.print("Arduino");
  u8g2.setCursor(40, 10);
  u8g2.print("Loading...");
}

void renderProgram() {
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawCircle(64, 32, 20);
  u8g2.drawLine(64, 27, 64, 37);
  u8g2.drawLine(59, 32, 69, 32);
  u8g2.setCursor(40, 55);
  u8g2.print(activeProgram->name);
  u8g2.setCursor(40, 10);
  u8g2.print("Running...");
}

void renderPosition() {
  u8g2.setFont(u8g2_font_ncenB08_tr);

  u8g2.setCursor(0, 10);
  u8g2.print(currentPosition.pos->name);

  u8g2.drawLine(0, 12, 128, 12);

  u8g2.setCursor(5, 24);
  u8g2.print("old: ");
  u8g2.print(currentPosition.value);
  u8g2.setCursor(5, 36);
  u8g2.print("new: ");
  u8g2.print(newPosition.value);
}

void printStringFromProgmem(const char* str) {
    char buffer[30];
    strcpy_P(buffer, str);
    u8g2.print(buffer);
}

void printSubMenuFromProgmem(MenuPage* subMenuPtr) {
  MenuPage subMenu;
  memcpy_P(&subMenu, subMenuPtr, sizeof(MenuPage));
  printStringFromProgmem(subMenu.title);
}

void printProgramFromProgmem(Program* programPtr) {
  Program program;
  memcpy_P(&program, programPtr, sizeof(Program));
  printStringFromProgmem(program.name);
}

void printPositionFromProgmem(Position* positionPtr) {
  Position position;
  memcpy_P(&position, positionPtr, sizeof(Position));
  printStringFromProgmem(position.name);
}
